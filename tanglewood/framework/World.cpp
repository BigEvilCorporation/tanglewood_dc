///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		World.cpp
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Encapsulates and processes the world environment
///////////////////////////////////////////////////////////////////

#include "World.h"
#include "Types.h"
#include "Physics.h"
#include "Constants.h"
#include "Globals.h"
#include "Palettes.h"
#include "Shaders.h"
#include "Camera.h"
#include "ObjectFactory.h"

#include <ion/core/debug/Debug.h>
#include <ion/core/string/String.h>
#include <ion/core/utils/STL.h>

//TODO: Move
#include "tanglewood/Player.h"
#include "tanglewood/PlayerController.h"
const char* nymnObjectName = "nymn";

World::World()
{
	m_planeFg = NULL;
	m_planeBg = NULL;
	m_levelIdx = -1;

	m_physicsWorld = new PhysicsWorld();

	//Init effects
	m_paletteLerpSpeed = 0.0f;
	m_paletteLerpTimer = 0.0f;
}

World::~World()
{
	DeleteGameObjects();

#if USE_PALETTE_TEXTURES
	if (Assets::Palettes::World::shared)
	{
		delete Assets::Palettes::World::shared;
		Assets::Palettes::World::shared = nullptr;
	}
#endif

	if (m_physicsWorld)
	{
		delete m_physicsWorld;
	}

	if(m_planeFg)
	{
		delete m_planeFg;
	}

	if(m_planeBg)
	{
		delete m_planeBg;
	}
}

bool World::LoadSprites()
{
	//Build used and unused sprite lists
	std::set<std::string> unusedActors;
	std::vector<std::string> usedActors;

	for(std::map<std::string, Actor>::const_iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		unusedActors.insert(ion::string::ToLower(it->second.GetName()));
	}

	for (TGameObjectPosMap::const_iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
	{
		TGameObjectTypeMap::const_iterator typeIt = m_gameObjectTypes.find(it->first);
		if (typeIt != m_gameObjectTypes.end())
		{
			const GameObjectType& gameObjType = typeIt->second;
			const std::string& typeName = gameObjType.GetName();

			//Find in object registry
			std::vector<ObjectFactory::ObjectRegistryEntry>::const_iterator regIt = std::find_if(ObjectFactory::objectRegistry.begin(), ObjectFactory::objectRegistry.end(), [&typeName](const ObjectFactory::ObjectRegistryEntry& rhs) { return typeName == rhs.typeName; });

			if (regIt != ObjectFactory::objectRegistry.end())
			{
				const ObjectFactory::ObjectRegistryEntry& registryEntry = (*regIt);

				//Add to used list, remove from unused list
				Actor* actor = nullptr;
				if (!registryEntry.actorName.empty())
				{
					usedActors.push_back(registryEntry.actorName);
					unusedActors.erase(ion::string::ToLower(registryEntry.actorName));
				}
			}
			else
			{
				ion::debug::log << "World::LoadSprites() - Could not find object factory for type \'" << typeName << "\'" << ion::debug::end;
			}
		}
		else
		{
			ion::debug::log << "World::LoadSprites() - Could not find game object type id \'" << it->first << "\'" << ion::debug::end;
		}
	}

	//Dump unused actors
	for (std::set<std::string>::const_iterator it = unusedActors.begin(), end = unusedActors.end(); it != end; ++it)
	{
		m_actors.erase(*it);
	}

	//Load used actors
	for(int i = 0; i < usedActors.size(); i++)
	{
		
		LoadSprite(usedActors[i]);
	}

	return true;
}

bool World::LoadSprite(const std::string& name)
{
	//If not already loaded
	if (!FindActor(name))
	{
		//TODO: move to constants
		const std::string directory = "assets/sprites";

		std::stringstream filename;
		filename << directory << "/" << name << ".bee";

		ion::io::File file(filename.str(), ion::io::File::eOpenRead);
		if (file.IsOpen())
		{
			//New actor
			Actor& actor = m_actors.insert(std::make_pair(ion::string::ToLower(name), Actor())).first->second;

			//Serialise
			ion::io::Archive archive(file, ion::io::Archive::Direction::In);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			archive.Serialise(actor, "actor");

			return true;
		}
		else
		{
			ion::debug::log << "World::LoadSprite() - Could not load sprite actor \'" << filename.str() << "\'" << ion::debug::end;
		}
	}

	return false;
}

bool World::LoadChapterData(const LevelDescriptor& level)
{
#if USE_PALETTE_TEXTURES
	//TODO: Doesn't belong here
	if (Assets::Palettes::World::shared)
	{
		delete Assets::Palettes::World::shared;
	}
#endif

	//Load tileset
	ion::debug::log << "Loading " << level.tilesetName << ion::debug::end;
	ion::io::File tilesetFile(level.tilesetName , ion::io::File::eOpenRead);
	if (tilesetFile.IsOpen())
	{
		ion::io::Archive archive(tilesetFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		m_tileset.Serialise(archive);
		tilesetFile.Close();
	}
	else
	{
		ion::debug::log << "Failed to load " << level.tilesetName << ion::debug::end;
	}

	//Load palettes
	ion::debug::log << "Loading " << level.palettesName << ion::debug::end;
	ion::io::File palettesFile(level.palettesName, ion::io::File::eOpenRead);
	if (palettesFile.IsOpen())
	{
		ion::io::Archive archive(palettesFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_palettes, "palettes");
		palettesFile.Close();
	}
	else
	{
		ion::debug::log << "Failed to load " << level.palettesName << ion::debug::end;
	}

	//Load collision tileset
	ion::debug::log << "Loading " << level.collisionTilesName << ion::debug::end;
	m_physicsWorld->LoadCollisionTileset(level.collisionTilesName);

#if USE_PALETTE_TEXTURES
	//Get time of day palettes
	Assets::Palettes::World::day = m_palettes[0];
	Assets::Palettes::World::dusk = m_palettes[1];
	Assets::Palettes::World::night = m_palettes[2];
#endif

	return true;
}

bool World::LoadActData(const LevelDescriptor& level)
{
	//TODO: Shared tileset texture

	//Load fg map
	ion::debug::log << "Loading " << level.tileMapFgName << ion::debug::end;
	ion::io::File mapFileFg(level.tileMapFgName, ion::io::File::eOpenRead);
	if (mapFileFg.IsOpen())
	{
		//Serialise
		ion::io::Archive archive(mapFileFg, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_mapSizeTilesFg, "size");
		archive.Serialise(m_tileMapFg, "tileMap");
		mapFileFg.Close();

		//Create fg plane from map
		m_planeFg = new Plane(m_tileset, m_tileMapFg, m_mapSizeTilesFg, ion::Vector2i(Constants::MegaDrive::planeWidthTiles, Constants::MegaDrive::planeHeightTiles), m_palettes[0]);
	}
	else
	{
		ion::debug::log << "Failed to load " << level.tileMapFgName << ion::debug::end;
	}

	//Load bg map
	ion::debug::log << "Loading " << level.tileMapBgName << ion::debug::end;
	ion::io::File mapFileBg(level.tileMapBgName, ion::io::File::eOpenRead);
	if (mapFileBg.IsOpen())
	{
		//Serialise
		ion::io::Archive archive(mapFileBg, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_mapSizeTilesBg, "size");
		archive.Serialise(m_tileMapBg, "tileMap");
		mapFileBg.Close();

		//Create fg plane from map
		m_planeBg = new Plane(m_tileset, m_tileMapBg, m_mapSizeTilesBg, ion::Vector2i(Constants::MegaDrive::planeWidthTiles, Constants::MegaDrive::planeHeightTiles), m_palettes[0]);
	}
	else
	{
		ion::debug::log << "Failed to load " << level.tileMapBgName << ion::debug::end;
	}

	//Load game objects
	ion::debug::log << "Loading " << level.gameObjectsName << ion::debug::end;
	ion::io::File gameObjMapFile(level.gameObjectsName, ion::io::File::eOpenRead);
	if (gameObjMapFile.IsOpen())
	{
		ion::io::Archive archive(gameObjMapFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_gameObjects, "gameObjects");
	}
	else
	{
		ion::debug::log << "Failed to load " << level.gameObjectsName << ion::debug::end;
	}

	//Load physics map
	ion::debug::log << "Loading " << level.collisionMapName << ion::debug::end;
	m_physicsWorld->LoadCollisionMap(level.collisionMapName);

	//Get map size
	m_mapSizeFg.x = m_mapSizeTilesFg.x * 8;
	m_mapSizeFg.y = m_mapSizeTilesFg.y * 8;
	m_mapSizeBg.x = m_mapSizeTilesBg.x * 8;
	m_mapSizeBg.y = m_mapSizeTilesBg.y * 8;

	//Set plane palettes
	m_currentPalette = Assets::Palettes::World::day;

#if USE_PALETTE_TEXTURES
	Assets::Palettes::World::shared = PaletteTools::CreatePaletteTexture(m_currentPalette);

	m_planeFg->SetPaletteTexture(Assets::Palettes::World::shared);
	m_planeBg->SetPaletteTexture(Assets::Palettes::World::shared);
#endif

	//Get bg colour
	const Colour& bgColour = m_palettes[0].GetColour(0);
	m_bgColour.r = bgColour.GetRed() / 255.0f;
	m_bgColour.g = bgColour.GetGreen() / 255.0f;
	m_bgColour.b = bgColour.GetBlue() / 255.0f;
	m_bgColour.a = 1.0f;

	//Set plane scroll types
	m_planeBg->SetEdgeBehaviour(Plane::EdgeBehaviour::Wrap, Plane::EdgeBehaviour::Clamp);
	m_planeFg->SetEdgeBehaviour(Plane::EdgeBehaviour::Clamp, Plane::EdgeBehaviour::Clamp);

	return true;
}

bool World::LoadGameObjectTypes(const std::string& name)
{
	ion::debug::log << "Loading " << name << ion::debug::end;
	ion::io::File file(name, ion::io::File::eOpenRead);
	if (file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_gameObjectTypes, "gameObjectTypes");
		return true;
	}

	ion::debug::log << "Failed to load " << name << ion::debug::end;
	
	return false;
}

bool World::CreateGameObjects()
{
    for(TGameObjectPosMap::const_iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
    {
        //Get game object type
		TGameObjectTypeMap::const_iterator typeIt = m_gameObjectTypes.find(it->first);
		if(typeIt != m_gameObjectTypes.end())
		{
			const GameObjectType& gameObjType = typeIt->second;

            for(int i = 0; i < it->second.size(); i++)
            {
                //Create entity
                if(Entity* entity = ObjectFactory::Create(*this, m_actors, it->second[i].m_gameObject, gameObjType))
                {
					//Add to typed list
					AddEntity<Entity>(*entity);

					//Add to flat list
					m_entities.push_back(entity);
                }
            }
        }
		else
		{
			ion::debug::log << "Failed to find game object type " << it->first << " for game objects: " << ion::debug::end;

			for (int i = 0; i << it->second.size(); i++)
			{
				ion::debug::log << "  " << it->second[i].m_gameObject.GetName() << ion::debug::end;
			}

			ion::debug::Assert(false, "");
		}
    }

    //Find Nymn, create player controller
    //TODO: move
	if (Globals::Players::playerController1)
	{
		delete Globals::Players::playerController1;
		Globals::Players::playerController1 = nullptr;
	}

	std::vector<Player*> players = GetEntities<Player>();
	if (!players.empty())
	{
		Globals::Players::player1 = players[0];
		Globals::Players::playerController1 = new PlayerController(*players[0]);

		//Set default camera target
		Globals::Game::camera->SetTarget(Globals::Players::player1);
	}
	else
    {
        ion::debug::error << "Could not find Nymn" << ion::debug::end;
        return false;
    }

#if 0
	ion::debug::log << "Mem used before sprite sheet deletion: " << ion::debug::GetRAMUsed() << ion::debug::end;

	for (std::map<ActorId, Actor>::iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		for (TSpriteSheetMap::iterator sheetIt = it->second.GetSpriteSheets().begin(), sheetEnd = it->second.GetSpriteSheets().end(); sheetIt != sheetEnd; ++sheetIt)
		{
			sheetIt->second.ClearFrames();
		}
	}

	ion::debug::log << "Mem used after sprite sheet deletion: " << ion::debug::GetRAMUsed() << ion::debug::end;
#endif

	return true;
}

void World::DeleteGameObjects()
{
	for (int i = 0; i < m_entities.size(); i++)
	{
		delete m_entities[i];
	}

	m_entitiesByType.clear();
	m_entities.clear();
}

void World::Reset()
{
	//Delete all entities
	DeleteGameObjects();

	//Reset physics world
	m_physicsWorld->RemoveAllObjects();

	//Recreate game objects
	CreateGameObjects();
}

void World::Update(float deltaTime, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad)
{
    //Update player controller
    if(Globals::Players::playerController1)
    {
		Globals::Players::playerController1->Update(deltaTime, keyboard, gamepad);
    }

	if (!Globals::Players::playerController1 || !Globals::Players::playerController1->m_debugMove)
	{
		//Step physics world
		m_physicsWorld->Step(deltaTime);

		//Update game objects
		for (int i = 0; i < m_entities.size(); i++)
		{
			if (m_entities[i]->m_active)
			{
				m_entities[i]->Update(deltaTime);
			}
		}
	}

	//Update camera
	Globals::Game::camera->Update(deltaTime, m_mapSizeFg.y);

	//Update background scroll
	m_planeBg->m_scroll.x = Globals::Game::camera->GetWorldPos().x / 2.0f;
	//m_planeBg->m_scroll.y = Globals::Game::camera->GetWorldPos().y / 2.0f;

	//Update effects
	m_fader.Update(deltaTime);
	UpdatePaletteLerp(deltaTime);
}

void World::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, const ion::render::Viewport& viewport)
{
	//Get camera matrix
	ion::Matrix4 cameraInv = camera.GetTransform().GetInverse();

	//TODO: One plane per draw priority (store sprites on plane)
	const std::vector<SpriteObj*>& sprites = GetEntities<SpriteObj>();

	//Draw planes (low prio)
	m_planeBg->Render(renderer, nullptr, PlanePriority::PlaneBLow);
	m_planeFg->Render(renderer, &camera, PlanePriority::PlaneALow);

	//Draw sprites
	for (int i = 0; i < sprites.size(); i++)
	{
		if (sprites[i]->m_planePriority == PlanePriority::SpriteLow)
		{
			sprites[i]->Render(renderer, camera, viewport, cameraInv, m_mapSizeFg);
		}
	}

	//Draw planes (high prio)
	//m_planeBg->Render(renderer, nullptr, PlanePriority::PlaneBHigh);
	//m_planeFg->Render(renderer, &camera, PlanePriority::PlaneAHigh);

	//Draw sprites
	for (int i = 0; i < sprites.size(); i++)
	{
		if (sprites[i]->m_planePriority == PlanePriority::SpriteHigh)
		{
			sprites[i]->Render(renderer, camera, viewport, cameraInv, m_mapSizeFg);
		}
	}

	//Draw fade plane
#if !defined ION_PLATFORM_DREAMCAST
	//TODO: Vertex colours wrong on Dreamcast
	m_fader.Render(renderer);
#endif

#if defined DEBUG
	for (int i = 0; i < m_entities.size(); i++)
	{
		m_entities[i]->DebugDrawBounds(renderer, camera, viewport, cameraInv, m_mapSizeFg);
	}
#endif
}

void World::SetCameraPosition(const ion::Vector2& position)
{
	Globals::Game::camera->SetWorldPos(position);
}

void World::PreStreamMap()
{
	if (Globals::Players::playerController1)
	{
		//Set initial camera pos
		ion::Vector2 playerPos = Globals::Players::player1->GetWorldCentre();
		SetCameraPosition(ion::Vector2(playerPos.x, m_mapSizeFg.y - playerPos.y));

		if (m_planeFg)
		{
			m_planeFg->PreStream(Globals::Game::camera->GetRenderCamera());
		}

		if (m_planeBg)
		{
			m_planeBg->PreStream(Globals::Game::camera->GetRenderCamera());
		}
	}
}

const Actor* World::FindActor(const std::string& name) const
{
	std::map<std::string, Actor>::const_iterator it = m_actors.find(ion::string::ToLower(name));
	if (it != m_actors.end())
	{
		return &it->second;
	}

	return nullptr;
}

void World::ResetFader()
{
	m_fader.ResetToBlack();
}

bool World::BeginFade(float speed)
{
	return m_fader.BeginFade(speed);
}

bool World::IsFading() const
{
	return m_fader.IsFading();
}

void World::SetPalette(const Palette& palette)
{
	m_currentPalette = palette;
	PaletteTools::WritePaletteTexture(palette, Assets::Palettes::World::shared);
}

const Palette& World::GetPalette() const
{
	return m_currentPalette;
}

void World::BeginPaletteLerp(const Palette& dest, float speed)
{
	m_sourcePalette = m_currentPalette;
	m_currentPalette = dest;
	m_paletteLerpSpeed = speed;
	m_paletteLerpTimer = 0.0f;
}

void World::UpdatePaletteLerp(float deltaTime)
{
#if USE_PALETTE_TEXTURES
	if (m_paletteLerpSpeed > 0.0f)
	{
		m_paletteLerpTimer += m_paletteLerpSpeed * deltaTime;

		if (m_paletteLerpTimer >= 1.0f)
		{
			m_paletteLerpTimer = 1.0f;
			m_paletteLerpSpeed = 0.0f;
		}

		Palette palette;
		PaletteTools::BlendPalettes(m_sourcePalette, m_currentPalette, palette, m_paletteLerpTimer);
		PaletteTools::WritePaletteTexture(palette, Assets::Palettes::World::shared);
	}
#endif
}
