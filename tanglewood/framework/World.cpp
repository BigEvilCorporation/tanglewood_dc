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
#include "ObjectFactory.h"

#include <ion/core/debug/Debug.h>
#include <ion/core/string/String.h>
#include <ion/core/utils/STL.h>

//TODO: Move
#include "tanglewood/Player.h"
const char* nymnObjectName = "nymn";

World::World()
{
	m_stampSet = NULL;
	m_planeFg = NULL;
	m_planeBg = NULL;
	m_playerController = NULL;
	m_levelIdx = -1;

	m_physicsWorld = new PhysicsWorld();

	//Init effects
	m_fader = 0.0f;
	m_fadeSpeed = 0.0f;
	m_fadeQuad = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2(Globals::Rendering::gameCanvasWidth / 2, Globals::Rendering::gameCanvasHeight / 2));
	m_fadeMaterial = new ion::render::Material();
	m_fadeMaterial->SetDiffuseColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f));

#if defined ION_RENDERER_SHADER
	m_fadeMaterial->SetVertexShader(Assets::Shaders::FlatColoured::vertexShader.Get());
	m_fadeMaterial->SetPixelShader(Assets::Shaders::FlatColoured::pixelShader.Get());
#endif

	m_paletteLerpSpeed = 0.0f;
	m_paletteLerpTimer = 0.0f;
}

World::~World()
{
	DeleteGameObjects();

	delete m_fadeQuad;
	delete m_fadeMaterial;

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

	if(m_stampSet)
	{
		delete m_stampSet;
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

bool World::LoadSprites(const std::string& name)
{
	//Load sprite data from Beehive project file
	ion::io::File file(name, ion::io::File::eOpenRead);
	if (file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.Serialise(m_actors, "actors");
		return true;
	}
	else
	{
		ion::debug::error << "Error loading sprite data " << name << ion::debug::end;
		return false;
	}
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
	ion::io::File tilesetFile(level.tilesetName , ion::io::File::eOpenRead);
	if (tilesetFile.IsOpen())
	{
		ion::io::Archive archive(tilesetFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		m_tileset.Serialise(archive);
		tilesetFile.Close();
	}

	//Load stamps
	ion::io::File stampsFile(level.stampsName, ion::io::File::eOpenRead);
	if (stampsFile.IsOpen())
	{
		ion::io::Archive archive(stampsFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_stamps, "stamps");
		stampsFile.Close();
	}

	//Load palettes
	ion::io::File palettesFile(level.palettesName, ion::io::File::eOpenRead);
	if (palettesFile.IsOpen())
	{
		ion::io::Archive archive(palettesFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_palettes, "palettes");
		palettesFile.Close();
	}

	//Load collision tileset
	m_physicsWorld->LoadCollisionTileset(level.collisionTilesName);

	//Create stamp set
	m_stampSet = new StampSet(m_stamps, m_tileset, m_palettes[0]);

#if USE_PALETTE_TEXTURES
	//Get time of day palettes
	Assets::Palettes::World::day = m_palettes[0];
	Assets::Palettes::World::dusk = m_palettes[1];
	Assets::Palettes::World::night = m_palettes[2];

	//Set stamp palettes
	m_currentPalette = Assets::Palettes::World::day;
	Assets::Palettes::World::shared = PaletteTools::CreatePaletteTexture(m_currentPalette);

	for (std::map<StampId, StampRenderer>::iterator it = m_stampSet->m_stamps.begin(), end = m_stampSet->m_stamps.end(); it != end; ++it)
	{
		it->second.SetPaletteTexture(Assets::Palettes::World::shared);
	}
#endif

	return true;
}

bool World::LoadActData(const LevelDescriptor& level)
{
	//Load FG stamp map
	ion::io::File stampMapFileFg(level.stampMapFgName, ion::io::File::eOpenRead);
	if (stampMapFileFg.IsOpen())
	{
		ion::io::Archive archive(stampMapFileFg, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_mapSizeTilesFg, "mapSizeTiles");
		archive.Serialise(m_stampMapFg, "stamps");
		stampMapFileFg.Close();
	}

	//Load BG stamp map
	ion::io::File stampMapFileBg(level.stampMapBgName, ion::io::File::eOpenRead);
	if (stampMapFileBg.IsOpen())
	{
		ion::io::Archive archive(stampMapFileBg, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_mapSizeTilesBg, "mapSizeTiles");
		archive.Serialise(m_stampMapBg, "stamps");
		stampMapFileBg.Close();
	}

	//Load game objects
	ion::io::File gameObjMapFile(level.gameObjectsName, ion::io::File::eOpenRead);
	if (gameObjMapFile.IsOpen())
	{
		ion::io::Archive archive(gameObjMapFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_gameObjects, "gameObjects");
	}

	//Load physics map
	m_physicsWorld->LoadCollisionMap(level.collisionMapName);

	//Create fg plane from map
	m_planeFg = new Plane(m_stampMapFg, *m_stampSet);

	//Create bg plane from map
	m_planeBg = new Plane(m_stampMapBg, *m_stampSet);

	//Get map size
	m_mapSizeFg.x = m_mapSizeTilesFg.x * 8;
	m_mapSizeFg.y = m_mapSizeTilesFg.y * 8;
	m_mapSizeBg.x = m_mapSizeTilesBg.x * 8;
	m_mapSizeBg.y = m_mapSizeTilesBg.y * 8;

	//TEMP
	m_planeBg->m_drawOffset.x = -(64 * 8) / 2;
	m_planeBg->m_drawOffset.y = -(32 * 8) / 2;

	//Get bg colour
	const Colour& bgColour = m_palettes[0].GetColour(0);
	m_bgColour.r = bgColour.GetRed() / 255.0f;
	m_bgColour.g = bgColour.GetGreen() / 255.0f;
	m_bgColour.b = bgColour.GetBlue() / 255.0f;
	m_bgColour.a = 1.0f;

	return true;
}

bool World::LoadGameObjectTypes(const std::string& name)
{
	ion::io::File file(name, ion::io::File::eOpenRead);
	if (file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_gameObjectTypes, "gameObjectTypes");
		return true;
	}
	
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
	std::vector<Player*> players = GetEntities<Player>();
	if (!players.empty())
	{
		Globals::Players::player1 = players[0];
		m_playerController = new PlayerController(*players[0]);
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
    if(m_playerController)
    {
        m_playerController->Update(deltaTime, keyboard, gamepad);
    }

	if (!m_playerController || !m_playerController->m_debugMove)
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

	//Centre camera on player
	if(m_playerController)
	{
		ion::Vector2 playerPos = m_playerController->GetCentre();
		SetCameraPosition(ion::Vector2(playerPos.x, m_mapSizeFg.y - playerPos.y));
	}

	//Update background scroll
	m_planeBg->m_scroll.x = m_cameraPos.x;
	m_planeBg->m_scroll.y = m_cameraPos.y;

	//Update effects
	UpdateFader(deltaTime);
	UpdatePaletteLerp(deltaTime);
}

void World::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, const ion::render::Viewport& viewport, const ion::Matrix4& cameraInv)
{
	//Get camera bounds
	Bounds cameraBounds;
	cameraBounds.topLeft.x = camera.GetPosition().x;
	cameraBounds.topLeft.y = m_mapSizeFg.y - camera.GetPosition().y - Constants::MegaDrive::screenHeight;
	cameraBounds.bottomRight.x = camera.GetPosition().x + Constants::MegaDrive::screenWidth;
	cameraBounds.bottomRight.y = m_mapSizeFg.y - camera.GetPosition().y;

	//TODO: One plane per draw priority (store sprites on plane)
	const std::vector<SpriteObj*>& sprites = GetEntities<SpriteObj>();

	//Draw planes
	m_planeBg->Render(renderer, cameraBounds, cameraInv, m_mapSizeBg, PlanePriority::PlaneBLow);
	m_planeFg->Render(renderer, cameraBounds, cameraInv, m_mapSizeFg, PlanePriority::PlaneALow);

	//Draw sprites
	for (int i = 0; i < sprites.size(); i++)
	{
		if (sprites[i]->m_planePriority == PlanePriority::SpriteLow)
		{
			sprites[i]->Render(renderer, camera, viewport, cameraInv, m_mapSizeFg);
		}
	}

	//Draw planes
	m_planeBg->Render(renderer, cameraBounds, cameraInv, m_mapSizeBg, PlanePriority::PlaneBHigh);
	m_planeFg->Render(renderer, cameraBounds, cameraInv, m_mapSizeFg, PlanePriority::PlaneAHigh);

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
	ion::Matrix4 quadMatrix;
	quadMatrix.SetTranslation(ion::Vector3(Globals::Rendering::windowWidth / 2, Globals::Rendering::windowHeight / 2, 0.0f));
	m_fadeMaterial->Bind(quadMatrix, ion::Matrix4(), renderer.GetProjectionMatrix());
	renderer.DrawVertexBuffer(m_fadeQuad->GetVertexBuffer(), m_fadeQuad->GetIndexBuffer());
	m_fadeMaterial->Unbind();
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
	//Calc ratio of window to screen size
	ion::Vector3 cameraZoom;
	cameraZoom.x = (float)Globals::Rendering::windowWidth / (float)Globals::Rendering::gameCanvasWidth;
	cameraZoom.y = (float)Globals::Rendering::windowHeight / (float)Globals::Rendering::gameCanvasHeight;
	cameraZoom.z = 1.0f;

	//Set camera zoom
	Globals::Game::camera->SetZoom(cameraZoom);

	//Compensate camera pos
	ion::Vector3 cameraPos;
	cameraPos.x = position.x - (float)Globals::Rendering::gameCanvasWidth / 2.0f;
	cameraPos.y = position.y - (float)Globals::Rendering::gameCanvasHeight / 2.0f;
	cameraPos.z = -0.1f;

	//Set camera pos
	Globals::Game::camera->SetPosition(cameraPos);

	m_cameraPos = position;
}

const Actor* World::FindActor(const std::string& name) const
{
	for (std::map<ActorId, Actor>::const_iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		if (ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return &it->second;
		}
	}

	return nullptr;
}

bool World::BeginFade(float speed)
{
	m_fadeSpeed = speed;
	return ((speed < 1.0f && m_fader > 0.0f) || (speed > 1.0f && m_fader < 1.0f));
}

bool World::IsFading() const
{
	return m_fadeSpeed != 0.0f;
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

void World::UpdateFader(float deltaTime)
{
	if (!ion::maths::IsZero(m_fadeSpeed))
	{
		m_fader += m_fadeSpeed * deltaTime;

		if (m_fader <= 0.0f)
		{
			m_fader = 0.0f;
			m_fadeSpeed = 0.0f;
		}
		else if (m_fader >= 1.0f)
		{
			m_fader = 1.0f;
			m_fadeSpeed = 0.0f;
		}

		m_fadeMaterial->SetDiffuseColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f - m_fader));
	}
}