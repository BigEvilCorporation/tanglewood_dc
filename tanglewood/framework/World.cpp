///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		World.cpp
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Encapsulates and processes the world environment
///////////////////////////////////////////////////////////////////

#include "World.h"
#include "Physics.h"
#include "Constants.h"
#include "Globals.h"
#include "Palettes.h"
#include "ObjectFactory.h"

#include <ion/core/debug/Debug.h>
#include <ion/core/string/String.h>
#include <ion/core/utils/STL.h>

//TODO: Move
#include "tanglewood/Player.h"
const char* nymnObjectName = "nymn";

World::World()
{
	m_levelData = NULL;
	m_currentMap = NULL;
	m_backgroundMap = NULL;
	m_stampSet = NULL;
	m_planeFg = NULL;
	m_planeBg = NULL;
	m_playerController = NULL;
	m_levelIdx = -1;

	m_physicsWorld = new PhysicsWorld();

	//Init effects
	m_fader = 0.0f;
	m_fadeSpeed = 0.0f;
	m_fadeQuad = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2(Globals::Rendering::windowWidth / 2, Globals::Rendering::windowHeight / 2));
	m_fadeQuad->SetColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f));
	m_fadeMaterial = new ion::render::Material();
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

	if(m_levelData)
	{
		delete m_levelData;
	}
}

bool World::LoadSprites(const std::string& name)
{
	//Load sprite data from Beehive project file
	ion::io::File file(name, ion::io::File::eOpenRead);
	if (file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::eIn);
		archive.Serialise(m_actors, "actors");
		return true;
	}
	else
	{
		ion::debug::error << "Error loading sprite data " << name << ion::debug::end;
		return false;
	}
}

bool World::LoadLevel(const std::string& name)
{
	if(m_levelData)
	{
		delete m_levelData;

#if USE_PALETTE_TEXTURES
		if (Assets::Palettes::World::shared)
		{
			delete Assets::Palettes::World::shared;
		}
#endif
	}

	//Load level data from Beehive project file
	m_levelData = new Project(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(!m_levelData->Load(name))
	{
		ion::debug::error << "Error loading level data " << name << ion::debug::end;
		return false;
	}

	//Load stamp set
	m_stampSet = new StampSet(*m_levelData);

#if USE_PALETTE_TEXTURES
	//Get time of day palettes
	Assets::Palettes::World::day = m_levelData->GetPaletteSlot(0)[0];
	Assets::Palettes::World::dusk = m_levelData->GetPaletteSlot(1)[0];
	Assets::Palettes::World::night = m_levelData->GetPaletteSlot(2)[0];

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

bool World::LoadAct(int levelIdx, const std::string& levelMap, const std::string& bgMap)
{
	m_levelIdx = levelIdx;

	//Find foreground/game data map
	m_currentMap = m_levelData->FindMap(levelMap);
	if(!m_currentMap)
	{
		ion::debug::error << "Error loading level map " << levelMap << ion::debug::end;
		return false;
	}

	//Find background map
	m_backgroundMap = m_levelData->FindMap(bgMap);
	if(!m_backgroundMap)
	{
		ion::debug::error << "Error loading background map " << bgMap << ion::debug::end;
		return false;
	}

	//Load physics world
	m_physicsWorld->LoadWorld(*m_levelData, levelMap);

	//Create fg plane from map
	m_planeFg = new Plane(*m_currentMap, *m_stampSet);

	//Create bg plane from map
	m_planeBg = new Plane(*m_backgroundMap, *m_stampSet);

	//Get map size
	m_mapSizeFg.x = m_currentMap->GetWidth() * 8;
	m_mapSizeFg.y = m_currentMap->GetHeight() * 8;
	m_mapSizeBg.x = m_backgroundMap->GetWidth() * 8;
	m_mapSizeBg.y = m_backgroundMap->GetHeight() * 8;

	//TEMP
	m_planeBg->m_drawOffset.x = -(64 * 8) / 2;
	m_planeBg->m_drawOffset.y = -(32 * 8) / 2;

	//Get bg colour
	const Colour& bgColour = m_levelData->GetPalette(0)->GetColour(0);
	m_bgColour.r = bgColour.GetRed() / 255.0f;
	m_bgColour.g = bgColour.GetGreen() / 255.0f;
	m_bgColour.b = bgColour.GetBlue() / 255.0f;
	m_bgColour.a = 1.0f;

	return true;
}

bool World::CreateGameObjects()
{
    const TGameObjectPosMap& gameObjects = m_currentMap->GetGameObjects();
    for(TGameObjectPosMap::const_iterator it = gameObjects.begin(), end = gameObjects.end(); it != end; ++it)
    {
        //Get game object type
        if(GameObjectType* gameObjType = m_levelData->GetGameObjectType(it->first))
        {
            for(int i = 0; i < it->second.size(); i++)
            {
                //Create entity
                if(Entity* entity = ObjectFactory::Create(*this, m_actors, it->second[i].m_gameObject, *gameObjType))
                {
					//Add to typed list
					AddEntity<Entity>(*entity);

					//Add to flat list
					m_entities.push_back(entity);
                }
            }
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

void World::Update(float deltaTime, ion::render::Camera& camera, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad, const ion::render::Window& window, const ion::Vector2i& screenSize)
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
		SetCameraPosition(ion::Vector2(playerPos.x, m_mapSizeFg.y - playerPos.y), camera, window, screenSize);
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
	//TODO: One plane per draw priority (store sprites on plane)
	const std::vector<SpriteObj*>& sprites = GetEntities<SpriteObj>();

	//Draw planes
	m_planeBg->Render(renderer, cameraInv, m_mapSizeBg, PlanePriority::PlaneALow);
	m_planeFg->Render(renderer, cameraInv, m_mapSizeFg, PlanePriority::PlaneALow);

	//Draw sprites
	for (int i = 0; i < sprites.size(); i++)
	{
		if (sprites[i]->m_planePriority == PlanePriority::SpriteLow)
		{
			sprites[i]->Render(renderer, camera, viewport, cameraInv, m_mapSizeFg);
		}
	}

	//Draw planes
	m_planeBg->Render(renderer, cameraInv, m_mapSizeBg, PlanePriority::PlaneAHigh);
	m_planeFg->Render(renderer, cameraInv, m_mapSizeFg, PlanePriority::PlaneAHigh);

	//Draw sprites
	for (int i = 0; i < sprites.size(); i++)
	{
		if (sprites[i]->m_planePriority == PlanePriority::SpriteHigh)
		{
			sprites[i]->Render(renderer, camera, viewport, cameraInv, m_mapSizeFg);
		}
	}

	//Draw fade plane
	ion::Matrix4 quadMatrix;
	quadMatrix.SetTranslation(ion::Vector3(Globals::Rendering::windowWidth / 2, Globals::Rendering::windowHeight / 2, 0.0f));
	m_fadeMaterial->Bind(quadMatrix, ion::Matrix4(), renderer.GetProjectionMatrix());
	renderer.DrawVertexBuffer(m_fadeQuad->GetVertexBuffer(), m_fadeQuad->GetIndexBuffer());
}

void World::SetCameraPosition(const ion::Vector2& position, ion::render::Camera& camera, const ion::render::Window& window, const ion::Vector2i& screenSize)
{
	//Calc ratio of window to screen size
	ion::Vector3 cameraZoom;
	cameraZoom.x = (float)window.GetClientAreaWidth() / (float)screenSize.x;
	cameraZoom.y = (float)window.GetClientAreaHeight() / (float)screenSize.y;
	cameraZoom.z = 1.0f;

	//Set camera zoom
	camera.SetZoom(cameraZoom);

	//Compensate camera pos
	ion::Vector3 cameraPos;
	cameraPos.x = position.x - (float)screenSize.x / 2.0f;
	cameraPos.y = position.y - (float)screenSize.y / 2.0f;
	cameraPos.z = -0.1f;

	//Set camera pos
	camera.SetPosition(cameraPos);

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

		m_fadeQuad->SetColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f - m_fader));
	}
}