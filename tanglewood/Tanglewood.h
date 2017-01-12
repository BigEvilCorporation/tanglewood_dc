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
#include <ion/beehive/Project.h>
#include <ion/beehive/Map.h>

#if defined ION_PLATFORM_WINDOWS
#include <ion/renderer/Window.h>
#endif

class Tanglewood : public ion::framework::Application
{
public:
	static const int s_defaultWindowWidth = 640;
	static const int s_defaultWindowHeight = 480;
	static const int s_defaultScreenWidth = 320;
	static const int s_defaultScreenHeight = 240;
	static const char* s_spriteDataFile;

	Tanglewood();
	virtual ~Tanglewood();

	virtual bool Initialise();
	virtual void Shutdown();
	virtual bool Update(float deltaTime);
	virtual void Render();

	bool LoadLevel(const std::string& name);
	bool LoadAct(const std::string& name);
	bool CreateGameObjects();

	//Set camera position, correcting for viewport size
	void SetCameraPosition(const ion::Vector2& position);

private:
	ion::render::Window* m_window;
	ion::render::Renderer* m_renderer;
	ion::render::Camera* m_camera;
	ion::render::Viewport* m_viewport;

	ion::Vector2i m_screenSize;

	//Beehive project files
	Project* m_levelData;
	Project* m_spriteData;
	
	//Beehive maps
	Map* m_currentMap;
	Map* m_backgroundMap;
};