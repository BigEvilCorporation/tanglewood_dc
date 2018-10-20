///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateLoading.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level loading state
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Level.h"
#include "framework/SpriteObj.h"

#include "effects/Fader.h"

#include <ion/core/thread/Thread.h>
#include <ion/core/thread/CriticalSection.h>
#include <ion/gamekit/StateManager.h>

#if defined ION_PLATFORM_DREAMCAST
#define THREADED_LOADING 0
#else
#define THREADED_LOADING 1
#endif

class StateLoading : public ion::gamekit::State
{
public:
	StateLoading(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager);
	virtual ~StateLoading();

	virtual void OnEnterState();
	virtual void OnLeaveState();
	virtual void OnPauseState();
	virtual void OnResumeState();

	virtual bool Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport);

private:
	//TODO: Use a secondary world
	void SetCameraPosition(const ion::Vector2& position);

	//Load thread
	class LoadingThread : public ion::thread::Thread
	{
	public:
		LoadingThread()
			: ion::thread::Thread("Loading")
		{
			m_running = true;
		}

		virtual void Entry();

		//TODO: Move to global resource load
		void LoadGlobalPalettes();

		bool m_running;
	};

	LoadingThread* m_loadingThread;
	bool m_fadingOut;

	//Loading screen assets
	//TODO: Use a secondary world
	std::map<ActorId, Actor> m_actors;
	World* m_loadingWorld;
	SpriteObj* m_loadingSprite;
	GameObjectType* m_loadingGameObjType;
	GameObject* m_loadingGameObj;
	ion::render::Camera m_loadingCamera;
	ion::Vector2 m_loadingScreenSize;
	Fader m_fader;
};