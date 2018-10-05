///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateFail.cpp
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	Failure/restart state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "StateFail.h"

#include "Globals.h"

StateFail::StateFail(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State("fail", stateManager, resourceManager)
{

}

StateFail::~StateFail()
{

}

void StateFail::OnEnterState()
{

}

void StateFail::OnLeaveState()
{

}

void StateFail::OnPauseState()
{

}

void StateFail::OnResumeState()
{

}

bool StateFail::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	//Wait for death animation to finish
	if (!Globals::Players::player1->GetCurrentAnimation() || Globals::Players::player1->GetCurrentAnimation()->GetState() == ion::render::Animation::eStopped)
	{
		//TODO: Fade down

		//Re-enter loading state
		m_stateManager.SwapState("loading");
	}

	return true;
}

void StateFail::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}