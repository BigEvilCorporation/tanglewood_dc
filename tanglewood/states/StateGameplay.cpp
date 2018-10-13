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

#include "Globals.h"

StateGameplay::StateGameplay(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State("gameplay", stateManager, resourceManager)
{

}

StateGameplay::~StateGameplay()
{

}

void StateGameplay::OnEnterState()
{
	//Begin level logic
	Globals::Game::level->Start();

	//Begin fade up
	Globals::Game::world->BeginFade(Constants::Flow::defaultFadeSpeed);
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
	Globals::Game::level->Update(deltaTime);

	//Check level ended
	if (!Globals::Game::level->IsRunning())
	{
		//End act/end level state
		Level::State levelState = Globals::Game::level->GetState();

		if(levelState == Level::State::ActEnded)
		{
			m_stateManager.SwapState("endact");
		}
		else if (levelState == Level::State::ChapterEnded)
		{
			m_stateManager.SwapState("endchapter");
		}
		
		return false;
	}

	//If player dead, enter failure state
	if (!Globals::Players::player1->m_alive)
	{
		m_stateManager.SwapState("fail");
		return false;
	}

	return true;
}

void StateGameplay::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}