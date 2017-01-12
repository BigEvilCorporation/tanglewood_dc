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

	//Load first level data file
	if(!LoadLevel(levelName))
	{
		ion::debug::error << "Could not load level data " << levelName << ion::debug::end;
		return false;
	}

	//Load first act
	if(!LoadAct(actName))
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
	//Update game objects
	nymn->Update(deltaTime);

	//Centre camera on Nymn
	SetCameraPosition(ion::Vector2(nymn->m_worldPos.x, nymn->m_worldPos.y));

	return m_window->Update();
}

void Tanglewood::Render()
{
	m_renderer->BeginFrame(*m_viewport, m_window->GetDeviceContext());
	m_renderer->ClearColour();
	m_renderer->ClearDepth();

#if defined ION_PLATFORM_DREAMCAST
	//TEMP
	m_renderer->SetFaceCulling(ion::render::Renderer::eNoCull);
#endif

	//Draw sprites
	ion::Matrix4 cameraInv = m_camera->GetTransform().GetInverse();
	nymn->Render(*m_renderer, cameraInv);

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

	return true;
}

bool Tanglewood::LoadAct(const std::string& name)
{
	//Find foreground/game data map
	m_currentMap = m_levelData->FindMap(name);
	if(!m_currentMap)
	{
		ion::debug::error << "Error loading map " << name << ion::debug::end;
		return false;
	}

	//Find background map
	//m_backgroundMap = m_levelData->FindMap(bgName);

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