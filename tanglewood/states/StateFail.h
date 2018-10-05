///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateFail.h
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	Failure/restart state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/World.h"
#include "framework/Level.h"

#include <ion/gamekit/StateManager.h>

class StateFail : public ion::gamekit::State
{
public:
	StateFail(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager);
	virtual ~StateFail();

	virtual void OnEnterState();
	virtual void OnLeaveState();
	virtual void OnPauseState();
	virtual void OnResumeState();

	virtual bool Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport);
};