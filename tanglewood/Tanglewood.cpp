///////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Tanglewood.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Tanglewood main game class
///////////////////////////////////////////////////

#include "Tanglewood.h"

#include <ion/core/debug/Debug.h>

const char* Tanglewood::s_spriteDataFile = "cd/sprites.bee";

//TODO: Move
#include "framework/PhysicsObj.h"
PhysicsObj* nymn;

Tanglewood::Tanglewood()
	: Application("Tanglewood")
{
	m_levelData = NULL;
	m_spriteData = NULL;
	m_currentMap = NULL;
	m_backgroundMap = NULL;
}

Tanglewood::~Tanglewood()
{

}

bool Tanglewood::Initialise()
{
	//Create window/renderer/camera/viewport
	m_window = ion::render::Window::Create("Tanglewood", s_defaultWindowWidth, s_defaultWindowHeight, false);
	m_renderer = ion::render::Renderer::Create(m_window->GetDeviceContext());
	m_camera = new ion::render::Camera();
	m_viewport = new ion::render::Viewport(s_defaultWindowWidth, s_defaultWindowHeight, ion::render::Viewport::eOrtho2DAbsolute);

	//Create input devices
	m_keyboard = new ion::input::Keyboard();

	//Set initial screen size
	m_screenSize.x = s_defaultScreenWidth;
	m_screenSize.y = s_defaultScreenHeight;

	//Set initial camera position
	SetCameraPosition(ion::Vector2(0.0f, 0.0f));

	//Load sprite data from Beehive project file
	m_spriteData = new Project(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(!m_spriteData->Load(s_spriteDataFile))
	{
		ion::debug::error << "Error loading sprite data " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	const char* levelName = "cd/l1.bee";
	const char* actName = "l1a1";
	const char* bgName = "l1bg";

	//Load first level data file
	if(!LoadLevel(levelName))
	{
		ion::debug::error << "Could not load level data " << levelName << ion::debug::end;
		return false;
	}

	//Load first act
	if(!LoadAct(actName, bgName))
	{
		ion::debug::error << "Could not load act " << actName << ion::debug::end;
		return false;
	}

	//Create game objects
	if(!CreateGameObjects())
	{
		ion::debug::error << "Could not create game objects for " << actName << ion::debug::end;
		return false;
	}

	return true;
}

void Tanglewood::Shutdown()
{
	if(m_keyboard)
	{
		delete m_keyboard;
	}

	if(m_levelData)
	{
		delete m_levelData;
	}

	if(m_spriteData)
	{
		delete m_spriteData;
	}

	if(m_viewport)
	{
		delete m_viewport;
	}

	if(m_camera)
	{
		delete m_camera;
	}

	if(m_renderer)
	{
		delete m_renderer;
	}

	if(m_window)
	{
		delete m_window;
	}
}

bool Tanglewood::Update(float deltaTime)
{
	//Update input devices
	m_keyboard->Update();

	//Update game objects
	nymn->Update(deltaTime);

#if defined ION_PLATFORM_WINDOWS && defined DEBUG
	if(m_keyboard->KeyDown(DIK_UP))
	{
		m_cameraPos.y += 500.0f * deltaTime;
	}
	if(m_keyboard->KeyDown(DIK_DOWN))
	{
		m_cameraPos.y -= 500.0f * deltaTime;
	}
	if(m_keyboard->KeyDown(DIK_LEFT))
	{
		m_cameraPos.x -= 500.0f * deltaTime;
	}
	if(m_keyboard->KeyDown(DIK_RIGHT))
	{
		m_cameraPos.x += 500.0f * deltaTime;
	}
#else
	//Centre camera on Nymn
	SetCameraPosition(ion::Vector2(nymn->m_worldPos.x, nymn->m_worldPos.y));
#endif

	//Update background scroll
	m_planeBg->m_scroll.x = m_cameraPos.x;
	m_planeBg->m_scroll.y = m_cameraPos.y;

	SetCameraPosition(m_cameraPos);

	return m_window->Update();
}

void Tanglewood::Render()
{
	m_renderer->BeginFrame(*m_viewport, m_window->GetDeviceContext());
	m_renderer->ClearColour();
	m_renderer->ClearDepth();
	m_renderer->SetAlphaBlending(ion::render::Renderer::eTranslucent);
	m_renderer->SetFaceCulling(ion::render::Renderer::eNoCull);

	ion::Matrix4 cameraInv = m_camera->GetTransform().GetInverse();

	//Draw planes
	m_planeBg->Render(*m_renderer, cameraInv, m_mapSizeBg);
	m_planeFg->Render(*m_renderer, cameraInv, m_mapSizeFg);

	//Draw sprites
	nymn->Render(*m_renderer, cameraInv, m_mapSizeFg);

	m_renderer->SwapBuffers();
	m_renderer->EndFrame();
}

bool Tanglewood::LoadLevel(const std::string& name)
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

bool Tanglewood::LoadAct(const std::string& levelMap, const std::string& bgMap)
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
	m_viewport->SetClearColour(ion::Colour(bgColour.GetRed() / 255.0f, bgColour.GetGreen() / 255.0f, bgColour.GetBlue() / 255.0f, 1.0f));

	return true;
}

bool Tanglewood::CreateGameObjects()
{
	//Find Nymn game object in level data
	GameObject* gameObjNymn = m_currentMap->FindGameObject("l1a1_nymn");
	if(!gameObjNymn)
	{
		ion::debug::error << "Error loading Nymn game object" << ion::debug::end;
		return false;
	}

	//Create Nymn
	nymn = new PhysicsObj(*gameObjNymn, *m_levelData->GetGameObjectType(gameObjNymn->GetTypeId()));

	//Find actors in sprite data
	Actor* actorNymn = m_spriteData->FindActor("nymn");
	if(!actorNymn)
	{
		ion::debug::error << "Could not find actor Nymn in " << s_spriteDataFile << ion::debug::end;
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

void Tanglewood::SetCameraPosition(const ion::Vector2& position)
{
	//Calc ratio of window to screen size
	ion::Vector3 cameraZoom;
	cameraZoom.x = (float)m_window->GetClientAreaWidth() / (float)m_screenSize.x;
	cameraZoom.y = (float)m_window->GetClientAreaHeight() / (float)m_screenSize.y;
	cameraZoom.z = 1.0f;

	//Set camera zoom
	m_camera->SetZoom(cameraZoom);

	//Compensate camera pos
	ion::Vector3 cameraPos;
	cameraPos.x = position.x + (((float)m_screenSize.x - (float)m_window->GetClientAreaWidth()) / 2.0f);
	cameraPos.y = position.y + (((float)m_screenSize.y - (float)m_window->GetClientAreaHeight()) / 2.0f);
	cameraPos.z = -0.1f;
	m_camera->SetPosition(cameraPos);
}