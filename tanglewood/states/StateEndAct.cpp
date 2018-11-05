///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateEndAct.cpp
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	End of act state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "StateEndAct.h"
#include "StateLoading.h"

#include "Globals.h"

#include "LevelSystem.h"

StateEndAct::StateEndAct(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State("endact", stateManager, resourceManager)
{

}

StateEndAct::~StateEndAct()
{

}

void StateEndAct::OnEnterState()
{

}

void StateEndAct::OnLeaveState()
{

}

void StateEndAct::OnPauseState()
{

}

void StateEndAct::OnResumeState()
{

}

bool StateEndAct::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	//Update world
	Globals::Game::world->Update(deltaTime, *keyboard, *gamepad);

	//TODO: Fade out

	//Next level
	LevelSystem::AdvanceLevel();

	//Load it
	m_stateManager.SwapState("loading");

	return false;
}

void StateEndAct::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{
	//Render world
	Globals::Game::world->Render(renderer, camera, viewport);
}