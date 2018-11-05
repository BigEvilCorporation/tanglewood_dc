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
#include "Globals.h"
#include "LevelSystem.h"

#if defined ION_RENDERER_SHADER
#include "Shaders.h"
#endif

#include <ion/core/debug/Debug.h>
#include <ion/core/thread/Sleep.h>

#include <ion/io/FileDevice.h>

Tanglewood::Tanglewood()
	: Application("Tanglewood")
{

}

Tanglewood::~Tanglewood()
{

}

bool Tanglewood::Initialise()
{
	//TODO: Merge ion::io into ion::core and add default file devices in ion::platform::Initialise()
#if defined ION_PLATFORM_DREAMCAST
	ion::io::FileDevice::SetDefault(new ion::io::FileDevice("disc", "/cd", ion::io::FileDevice::eOptical, ion::io::FileDevice::eReadOnly));
#endif

	//Create resource manager
	m_resourceManager = new ion::io::ResourceManager();
	m_resourceManager->SetResourceDirectory<ion::render::Shader>("assets/shaders", ".ion.shader");

	Globals::Rendering::windowWidth = s_defaultWindowWidth;
	Globals::Rendering::windowHeight = s_defaultWindowHeight;
	Globals::Rendering::gameCanvasWidth = s_defaultScreenWidth;
	Globals::Rendering::gameCanvasHeight = s_defaultScreenHeight;

	//Create window/renderer/camera/viewport
	m_window = ion::render::Window::Create("Tanglewood", s_defaultWindowWidth, s_defaultWindowHeight, false);
	m_renderer = ion::render::Renderer::Create(m_window->GetDeviceContext());
	m_viewport = new ion::render::Viewport(s_defaultWindowWidth, s_defaultWindowHeight, ion::render::Viewport::eOrtho2DAbsolute);
	m_camera = new GameCamera();

	//Begin loading global resources
	LoadGlobalResources();

	//Clear screen
	m_renderer->SetClearColour(ion::Colour(0.0f, 0.0f, 0.0f));
	m_renderer->BeginFrame(*m_viewport, m_window->GetDeviceContext());
	m_renderer->ClearColour();
	m_renderer->ClearDepth();
	m_renderer->SwapBuffers();
	m_renderer->EndFrame();

	//Create input devices
	m_keyboard = new ion::input::Keyboard();
	m_gamepad = new ion::input::Gamepad();

	//Create state manager
	m_stateManager = new ion::gamekit::StateManager();

	//Create GUI
	m_gui = new ion::gui::GUI(ion::Vector2i(s_defaultWindowWidth, s_defaultWindowHeight));
	m_debugUI = new DebugUI(*m_gui, ion::Vector2i(), ion::Vector2i());
	//m_gui->AddWindow(*m_debugUI);

	//Wait for resource loading
	while (m_resourceManager->GetNumResourcesWaiting())
	{
		ion::thread::Sleep(5);
	}

	PostLoadGlobalResources();

	//Begin gameplay
	BeginGameplay();

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

	//Render gamestate
	m_stateManager->Render(*m_renderer, m_camera->GetRenderCamera(), *m_viewport);

	//Render UI
#if !defined ION_PLATFORM_DREAMCAST
	//TODO: KGL not happy about some of the OpenGL calls in here
	m_gui->Render(*m_renderer, *m_viewport);
#endif

	m_renderer->SwapBuffers();
	m_renderer->EndFrame();
}

void Tanglewood::LoadGlobalResources()
{
#if defined ION_RENDERER_SHADER
	Assets::Shaders::FlatColoured::pixelShader = m_resourceManager->GetResource<ion::render::Shader>("flatcoloured_p");
	Assets::Shaders::FlatColoured::vertexShader = m_resourceManager->GetResource<ion::render::Shader>("flatcoloured_v");

	Assets::Shaders::FlatTextured::pixelShader = m_resourceManager->GetResource<ion::render::Shader>("flattextured_p");
	Assets::Shaders::FlatTextured::vertexShader = m_resourceManager->GetResource<ion::render::Shader>("flattextured_v");

	Assets::Shaders::IndexTexture::pixelShader = m_resourceManager->GetResource<ion::render::Shader>("indextexture_p");
	Assets::Shaders::IndexTexture::vertexShader = m_resourceManager->GetResource<ion::render::Shader>("indextexture_v");
#endif

#if defined DEBUG
	Debug::Assets::LoadAll(*m_resourceManager);
#endif
}

void Tanglewood::PostLoadGlobalResources()
{
#if defined ION_RENDERER_SHADER
	Assets::Shaders::IndexTexture::Params::indexedTexture = Assets::Shaders::IndexTexture::pixelShader->CreateParamHndl<ion::render::Texture>("gIndexedSampler");
	Assets::Shaders::IndexTexture::Params::paletteTexture = Assets::Shaders::IndexTexture::pixelShader->CreateParamHndl<ion::render::Texture>("gPaletteSampler");
#endif

#if defined DEBUG
	Debug::Assets::Init();
#endif
}

void Tanglewood::BeginGameplay()
{
	//Advance level
	LevelSystem::AdvanceLevel();
	
	//Set camera
	Globals::Game::camera = m_camera;

	//Create game states
	new StateFail(*m_stateManager, *m_resourceManager);
	new StateEndAct(*m_stateManager, *m_resourceManager);
	new StateEndChapter(*m_stateManager, *m_resourceManager);
	new StateGameplay(*m_stateManager, *m_resourceManager);
	new StateLoading(*m_stateManager, *m_resourceManager);

	//Begin loading state
	m_stateManager->PushState("loading");
}

void Tanglewood::EndGameplay()
{
	//Delete gamestates
	m_stateManager->PopState();
	m_stateManager->DeleteStates();

	//Delete world
	delete Globals::Game::world;
	Globals::Game::world = nullptr;

	//Delete level
	delete Globals::Game::level;
	Globals::Game::level = nullptr;
}