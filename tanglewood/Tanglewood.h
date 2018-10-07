///////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Tanglewood.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Tanglewood main game class
///////////////////////////////////////////////////

#include <ion/core/bootstrap/Application.h>
#include <ion/renderer/Renderer.h>
#include <ion/renderer/Camera.h>
#include <ion/renderer/Viewport.h>
#include <ion/renderer/Window.h>
#include <ion/input/Keyboard.h>
#include <ion/input/Gamepad.h>
#include <ion/io/ResourceManager.h>
#include <ion/gui/GUI.h>
#include <ion/gamekit/FPSCounter.h>
#include <ion/gamekit/StateManager.h>

#include "states/StateLoading.h"
#include "states/StateGameplay.h"
#include "states/StateFail.h"
#include "states/StateEndAct.h"
#include "states/StateEndChapter.h"

#include "Debug.h"

#if defined ION_PLATFORM_WINDOWS
#include <ion/renderer/Window.h>
#endif

class Tanglewood : public ion::framework::Application
{
public:
	//Window size
	static const int s_defaultWindowWidth = 1024;
	static const int s_defaultWindowHeight = 768;

	//Mega Drive resolution
	static const int s_defaultScreenWidth = 320;
	static const int s_defaultScreenHeight = 240;

	Tanglewood();
	virtual ~Tanglewood();

	virtual bool Initialise();
	virtual void Shutdown();
	virtual bool Update(float deltaTime);
	virtual void Render();

	void LoadGlobalResources();
	void PostLoadGlobalResources();

	void BeginGameplay();
	void EndGameplay();

private:
	ion::render::Window* m_window;
	ion::render::Renderer* m_renderer;
	ion::render::Viewport* m_viewport;
	ion::render::Camera* m_camera;
	ion::input::Keyboard* m_keyboard;
	ion::input::Gamepad* m_gamepad;
	ion::io::ResourceManager* m_resourceManager;
	ion::gui::GUI* m_gui;
	ion::gamekit::StateManager* m_stateManager;

	ion::Vector2i m_screenSize;

	//Debugging
	DebugUI* m_debugUI;
	ion::gamekit::FPSCounter m_fpsCounter;
};