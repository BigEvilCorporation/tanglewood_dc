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

//TEMP
const char* spriteDataFile = "cd/sprites.bee";
const char* levelDataFile = "cd/l1.bee";
const char* actName = "l1a1";
const char* bgName = "l1bg";

Tanglewood::Tanglewood()
	: Application("Tanglewood")
{

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
	m_gamepad = new ion::input::Gamepad();

	//Set initial screen size
	m_screenSize.x = s_defaultScreenWidth;
	m_screenSize.y = s_defaultScreenHeight;

	//Create world
	m_world = new World();

	//Load sprite data from Beehive project file
	if(!m_world->LoadSprites(spriteDataFile))
	{
		return false;
	}

	//Load first level data file from Beehive project file
	if(!m_world->LoadLevel(levelDataFile))
	{
		return false;
	}

	//Load first act
	if(!m_world->LoadAct(actName, bgName))
	{
		return false;
	}

	//Create game objects
	if(!m_world->CreateGameObjects())
	{
		return false;
	}

	return true;
}

void Tanglewood::Shutdown()
{
	if(m_world)
	{
		delete m_world;
	}

	if(m_keyboard)
	{
		delete m_keyboard;
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
	m_gamepad->Update();

	//Update world
	m_world->Update(deltaTime, *m_camera, *m_keyboard, *m_gamepad, *m_window, m_screenSize);

	//Update window
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

	//Render world
	m_world->Render(*m_renderer, cameraInv);

	m_renderer->SwapBuffers();
	m_renderer->EndFrame();
}
