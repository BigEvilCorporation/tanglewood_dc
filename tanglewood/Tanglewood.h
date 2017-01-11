#include <ion/core/bootstrap/Application.h>
#include <ion/renderer/Renderer.h>
#include <ion/renderer/Camera.h>
#include <ion/renderer/Viewport.h>
#include <ion/renderer/Window.h>
#include <ion/beehive/Project.h>

#if defined ION_PLATFORM_WINDOWS
#include <ion/renderer/Window.h>
#endif

class Tanglewood : public ion::framework::Application
{
public:
	static const int s_defaultWindowWidth = 640;
	static const int s_defaultWindowHeight = 480;
	static const int s_defaultViewportWidth = 320;
	static const int s_defaultViewportHeight = 240;
	static const char* s_spriteDataFile;

	Tanglewood();
	virtual ~Tanglewood();

	virtual bool Initialise();
	virtual void Shutdown();
	virtual bool Update(float deltaTime);
	virtual void Render();

	bool LoadLevel(const std::string& name);

private:
	ion::render::Window* m_window;
	ion::render::Renderer* m_renderer;
	ion::render::Camera* m_camera;
	ion::render::Viewport* m_viewport;

	//Beehive project files
	Project* m_levelData;
	Project* m_spriteData;
};