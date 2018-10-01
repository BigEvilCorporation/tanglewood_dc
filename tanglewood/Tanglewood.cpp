///////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Tanglewood.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Tanglewood main game class
///////////////////////////////////////////////////

#include "Tanglewood.h"
#include "Constants.h"

#include <ion/core/debug/Debug.h>

//TEMP
#include "levels/L1A1.h"

Tanglewood::Tanglewood()
	: Application("Tanglewood")
{

}

Tanglewood::~Tanglewood()
{

}

bool Tanglewood::Initialise()
{
	//Create resource manager
	m_resourceManager = new ion::io::ResourceManager();

	//Create window/renderer/camera/viewport
	m_window = ion::render::Window::Create("Tanglewood", s_defaultWindowWidth, s_defaultWindowHeight, false);
	m_renderer = ion::render::Renderer::Create(m_window->GetDeviceContext());
	m_camera = new ion::render::Camera();
	m_viewport = new ion::render::Viewport(s_defaultWindowWidth, s_defaultWindowHeight, ion::render::Viewport::eOrtho2DAbsolute);

	//Create input devices
	m_keyboard = new ion::input::Keyboard();
	m_gamepad = new ion::input::Gamepad();

	//Create state manager
	m_stateManager = new ion::gamekit::StateManager();

	//Set initial screen size
	m_screenSize.x = s_defaultScreenWidth;
	m_screenSize.y = s_defaultScreenHeight;

	//Create GUI
	m_gui = new ion::gui::GUI(ion::Vector2i(s_defaultWindowWidth, s_defaultWindowHeight));
	m_debugUI = new DebugUI(*m_gui, ion::Vector2i(), ion::Vector2i());
	//m_gui->AddWindow(*m_debugUI);

	//Begin level 0
	BeginGameplay(0);

	return true;
}

void Tanglewood::Shutdown()
{
	if (m_stateManager)
	{
		delete m_stateManager;
	}

	if (m_debugUI)
	{
		delete m_debugUI;
	}

	if (m_gui)
	{
		delete m_gui;
	}

	//TODO: Move to gameplay game state
	if (m_level)
	{
		delete m_level;
	}

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

	if (m_resourceManager)
	{
		delete m_resourceManager;
	}
}

bool Tanglewood::Update(float deltaTime)
{
	//Update input devices
	m_keyboard->Update();
	m_gamepad->Update();

	//Update world
	m_world->Update(deltaTime, *m_camera, *m_keyboard, *m_gamepad, *m_window, m_screenSize);

	//Update gamestate
	m_stateManager->Update(deltaTime, m_keyboard, nullptr, m_gamepad);

	//Update UI
	m_gui->Update(deltaTime, m_keyboard, nullptr, m_gamepad);

	//Update FPS counter
	m_fpsCounter.Update();
	std::stringstream text;
	text.setf(std::ios::fixed, std::ios::floatfield);
	text.precision(2);
	text << "TANGLEWOOD :: " << m_fpsCounter.GetLastFPS() << " FPS";
	m_window->SetTitle(text.str());

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

	//Render gamestate
	m_stateManager->Render(*m_renderer, *m_camera, *m_viewport);

	//Render UI
	m_gui->Render(*m_renderer, *m_viewport);

	m_renderer->SwapBuffers();
	m_renderer->EndFrame();
}

void Tanglewood::BeginGameplay(int levelIdx)
{
	//Create world
	m_world = new World();

	//Create level
	//TODO: From level list
	static const LevelData levelData =
	{
		"cd/sprites.bee_sprites",
		"cd/l1.bee",
		"l1a1",
		"l1bg"
	};

	m_level = new L1A1(levelData);

	//Create game states
	m_stateGameplay = new StateGameplay(*m_world, *m_level, *m_stateManager, *m_resourceManager);
	m_stateLoading = new StateLoading(*m_world, *m_level, *m_stateGameplay, *m_stateManager, *m_resourceManager);

	//Begin loading state
	m_stateManager->PushState(*m_stateLoading);
}

void Tanglewood::EndGameplay()
{
	delete m_stateLoading;
	delete m_stateGameplay;
	delete m_level;
	delete m_world;
}