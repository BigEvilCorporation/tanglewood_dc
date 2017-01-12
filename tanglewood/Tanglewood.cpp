#include "Tanglewood.h"

#include <ion/core/debug/Debug.h>

const char* Tanglewood::s_spriteDataFile = "cd/sprites.bee";

//TODO: Move
#include "framework/Sprite.h"
Sprite sprite;

Tanglewood::Tanglewood()
	: Application("Tanglewood")
{
	m_levelData = NULL;
	m_spriteData = NULL;
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

	//Set initial camera position
	m_camera->SetPosition(ion::Vector3(-(float)s_defaultWindowWidth / 2.0f, -(float)s_defaultWindowHeight / 2.0f, -0.1f));

	//Load sprite data from Beehive project file
	m_spriteData = new Project(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(!m_spriteData->Load(s_spriteDataFile))
	{
		ion::debug::error << "Error loading sprite data " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	//Find actors
	Actor* actorNymn = m_spriteData->FindActor("nymn");
	if(!actorNymn)
	{
		ion::debug::error << "Could not find actor Nymn in " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	//Create render resources for actor
	sprite.LoadActor(*actorNymn);

	sprite.SetAnimation("run", "run");

	//Load first level
	//return LoadLevel("cd/lvl1.bee");
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
	sprite.Update(deltaTime);

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
	sprite.Render(*m_renderer, cameraInv);

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
