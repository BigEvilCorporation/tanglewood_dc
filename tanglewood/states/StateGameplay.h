///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateGameplay.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Gameplay state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/World.h"
#include "framework/Level.h"

#include <ion/gamekit/StateManager.h>

class StateGameplay : public ion::gamekit::State
{
public:
	StateGameplay(World& world, Level& level, ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager);
	virtual ~StateGameplay();

	virtual void OnEnterState();
	virtual void OnLeaveState();
	virtual void OnPauseState();
	virtual void OnResumeState();

	virtual bool Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport);

private:
	World& m_world;
	Level& m_level;
};