///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateEndChapter.h
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	End of chapter state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/World.h"
#include "framework/Level.h"

#include <ion/gamekit/StateManager.h>

class StateEndChapter : public ion::gamekit::State
{
public:
	StateEndChapter(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager);
	virtual ~StateEndChapter();

	virtual void OnEnterState();
	virtual void OnLeaveState();
	virtual void OnPauseState();
	virtual void OnResumeState();

	virtual bool Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport);

private:
	float m_fadeOutTimer;
	bool m_fadingOut;
};