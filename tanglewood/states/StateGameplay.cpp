///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateGameplay.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Gameplay state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "StateGameplay.h"

StateGameplay::StateGameplay(World& world, Level& level, ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State(stateManager, resourceManager)
	, m_world(world)
	, m_level(level)
{

}

StateGameplay::~StateGameplay()
{

}

void StateGameplay::OnEnterState()
{
	//Begin level logic
	m_level.Start();
}

void StateGameplay::OnLeaveState()
{

}

void StateGameplay::OnPauseState()
{

}

void StateGameplay::OnResumeState()
{

}

bool StateGameplay::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	//Update level logic
	m_level.Update(deltaTime);
	if (!m_level.IsRunning())
	{
		return false;
	}
}

void StateGameplay::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}