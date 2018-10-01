///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateLoading.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level loading state
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/World.h"
#include "framework/Level.h"

#include <ion/gamekit/StateManager.h>

class StateGameplay;

class StateLoading : public ion::gamekit::State
{
public:
	StateLoading(World& world, Level& level, StateGameplay& stateGameplay, ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager);
	virtual ~StateLoading();

	virtual void OnEnterState();
	virtual void OnLeaveState();
	virtual void OnPauseState();
	virtual void OnResumeState();

	virtual bool Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport);

private:
	World& m_world;
	Level& m_level;
	StateGameplay& m_stateGameplay;
};