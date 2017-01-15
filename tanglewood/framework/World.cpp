///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		World.cpp
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Encapsulates and processes the world environment
///////////////////////////////////////////////////////////////////

#include "World.h"
#include "Constants.h"

#include <ion/core/debug/Debug.h>

//TODO: Move
#include "PhysicsObj.h"
PhysicsObj* nymn;
const char* nymnObjectName = "l1a1_nymn";

World::World()
{
	m_levelData = NULL;
	m_spriteData = NULL;
	m_currentMap = NULL;
	m_backgroundMap = NULL;
	m_terrainTileset = NULL;
	m_collisionMap = NULL;
	m_stampSet = NULL;
	m_planeFg = NULL;
	m_planeBg = NULL;

	m_gravity = Constants::World::defaultGravity;
}

World::~World()
{
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

	if(m_spriteData)
	{
		delete m_spriteData;
	}
}

bool World::LoadSprites(const std::string& name)
{
	//Load sprite data from Beehive project file
	m_spriteData = new Project(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(!m_spriteData->Load(name))
	{
		ion::debug::error << "Error loading sprite data " << name << ion::debug::end;
		return false;
	}

	return true;
}

bool World::LoadLevel(const std::string& name)
{
	if(m_levelData)
	{
		delete m_levelData;
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

	return true;
}

bool World::LoadAct(const std::string& levelMap, const std::string& bgMap)
{
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

	//Get collision map
	m_collisionMap = &m_levelData->GetCollisionMap(m_levelData->FindMapId(levelMap));

	//Get terrain tileset
	m_terrainTileset = &m_levelData->GetTerrainTileset();

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
	//Find Nymn game object in level data
	GameObject* gameObjNymn = m_currentMap->FindGameObject(nymnObjectName);
	if(!gameObjNymn)
	{
		ion::debug::error << "Error loading Nymn game object" << ion::debug::end;
		return false;
	}

	//Create Nymn
	nymn = new PhysicsObj(*this, *gameObjNymn, *m_levelData->GetGameObjectType(gameObjNymn->GetTypeId()));

	//Find actors in sprite data
	Actor* actorNymn = m_spriteData->FindActor("nymn");
	if(!actorNymn)
	{
		ion::debug::error << "Could not find actor Nymn" <<  ion::debug::end;
		return false;
	}

	//Create render resources for Nymn
	nymn->LoadActor(*actorNymn);

	//Set default animation
	nymn->SetAnimation("run", "run");

	//Init camera pos
	m_cameraPos = nymn->m_worldPos;

	return true;
}

void World::Update(float deltaTime, ion::render::Camera& camera, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad, const ion::render::Window& window, const ion::Vector2i& screenSize)
{
	//Update game objects
	nymn->Update(deltaTime);

#if defined ION_PLATFORM_WINDOWS && defined DEBUG && 0
	if(keyboard.KeyDown(DIK_UP))
	{
		m_cameraPos.y += 500.0f * deltaTime;
	}
	if(keyboard.KeyDown(DIK_DOWN))
	{
		m_cameraPos.y -= 500.0f * deltaTime;
	}
	if(keyboard.KeyDown(DIK_LEFT))
	{
		m_cameraPos.x -= 500.0f * deltaTime;
	}
	if(keyboard.KeyDown(DIK_RIGHT))
	{
		m_cameraPos.x += 500.0f * deltaTime;
	}
	SetCameraPosition(m_cameraPos, camera, window, screenSize);
#else
#if defined ION_PLATFORM_WINDOWS
	if(keyboard.KeyDown(DIK_UP))
	{
		nymn->m_worldPos.y += 896.0f * deltaTime;
	}
	if(keyboard.KeyDown(DIK_DOWN))
	{
		nymn->m_worldPos.y -= 896.0f * deltaTime;
	}
	if(keyboard.KeyDown(DIK_LEFT) || (gamepad.GetLeftStick().x < 0.0f))
	{
		nymn->m_acceleration.x = -Constants::Player::defaultPlayerAcceleration.x;
	}
	else if(keyboard.KeyDown(DIK_RIGHT) || (gamepad.GetLeftStick().x > 0.0f))
	{
		nymn->m_acceleration.x = Constants::Player::defaultPlayerAcceleration.x;
	}
	else
	{
		nymn->m_acceleration.x = 0.0f;
	}
#elif defined ION_PLATFORM_DREAMCAST
	if(gamepad.GetLeftStick().x < 0.0f)
	{
		nymn->m_acceleration.x = -Constants::Player::defaultPlayerAcceleration.x;
	}
	else if(gamepad.GetLeftStick().x > 0.0f)
	{
		nymn->m_acceleration.x = Constants::Player::defaultPlayerAcceleration.x;
	}
	else
	{
		nymn->m_acceleration.x = 0.0f;
	}
#endif

	//Centre camera on Nymn
	ion::Vector2 nymnCentre(nymn->m_worldPos.x + (nymn->m_size.x / 2.0f), m_mapSizeFg.y - nymn->m_worldPos.y - (nymn->m_size.y / 2.0f));
	SetCameraPosition(nymnCentre, camera, window, screenSize);
#endif

	//Update background scroll
	m_planeBg->m_scroll.x = m_cameraPos.x;
	m_planeBg->m_scroll.y = m_cameraPos.y;
}

void World::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv)
{
	//Draw planes
	m_planeBg->Render(renderer, cameraInv, m_mapSizeBg);
	m_planeFg->Render(renderer, cameraInv, m_mapSizeFg);

	//Draw sprites
	nymn->Render(renderer, cameraInv, m_mapSizeFg);
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
	cameraPos.x = position.x + (((float)screenSize.x - (float)window.GetClientAreaWidth()) / 2.0f);
	cameraPos.y = position.y + (((float)screenSize.y - (float)window.GetClientAreaHeight()) / 2.0f);
	cameraPos.z = -0.1f;

	//Set camera pos
	camera.SetPosition(cameraPos);

	m_cameraPos = position;
}

int World::FindFloor(const ion::Vector2i& position, int maxSearchLength, u16& tileFlags) const
{
	//Position to starting tile
	ion::Vector2i tilePos(position.x / 8, position.y / 8);

	int tileHeight = 0;
	bool found = false;

	if(tilePos.x >= 0 && tilePos.x < m_collisionMap->GetWidth())
	{
		//X offset
		int offsetX = position.x % 8;

		int lengthSearched = 0;
		int solidTilesFound = 0;
		int hollowTilesFound = 0;

		while(!found && lengthSearched < maxSearchLength && tilePos.y >= 0 && tilePos.y < m_collisionMap->GetHeight())
		{
			//Assume hollow if no tile
			int height = 0;

			//Get terrain tile id
			TerrainTileId terrainTileId = m_collisionMap->GetTerrainTile(tilePos.x, tilePos.y);

			//Get terrain tile
			if(const TerrainTile* terrainTile = m_terrainTileset->GetTerrainTile(terrainTileId))
			{
				//Get height at offset
				height = (int)terrainTile->GetHeight(offsetX);
			}

			if(height == 8)
			{
				//Solid tile
				solidTilesFound++;

				if(hollowTilesFound > 0)
				{
					//Last tile was hollow, found terrain
					found = true;
				}
				else
				{
					//Search upwards
					tilePos.y--;
				}
			}
			else if(height == 0)
			{
				//Hollow tile
				hollowTilesFound++;

				if(solidTilesFound > 0)
				{
					//Last tile was solid, found terrain
					found = true;
				}
				else
				{
					//Search downwards
					tilePos.y++;
				}

				lengthSearched += 8;
			}
			else
			{
				//Found
				found = true;
			}

			tileHeight = height;
		}
	}

	if(found)
	{
		//Get flags
		tileFlags = m_collisionMap->GetCollisionTileFlags(tilePos.x, tilePos.y);

		//Tile to pixel space + total height accumulated
		return (tilePos.y * 8) - tileHeight;
	}
	else
	{
		//No terrain within search distance
		tileFlags = 0;
		return -1;
	}
}

int World::FindWall(const ion::Vector2i& position, int direction, int maxSearchLength) const
{
	//Position to starting tile
	ion::Vector2i tilePos(position.x / 8, position.y / 8);
	u32 flags = 0;

	if(tilePos.y >= 0 && tilePos.y < m_collisionMap->GetHeight())
	{
		flags = m_collisionMap->GetCollisionTileFlags(tilePos.x, tilePos.y);
		int lengthSearched = 0;

		while((flags & eCollisionTileFlagSolid) == 0 && lengthSearched < maxSearchLength)
		{
			if(tilePos.x >= 0 && tilePos.x < m_collisionMap->GetWidth())
			{
				tilePos.x += direction;
				flags = m_collisionMap->GetCollisionTileFlags(tilePos.x, tilePos.y);
			}

			lengthSearched += 8;
		}
	}

	if((flags & eCollisionTileFlagSolid) != 0)
	{
		return (tilePos.x * 8) + ((direction < 0) ? 8 : 0);
	}
	else
	{
		return -1;
	}
}