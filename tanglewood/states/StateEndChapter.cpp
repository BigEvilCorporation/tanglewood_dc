///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateEndChapter.cpp
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	End of chapter state
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "StateEndChapter.h"

#include "Constants.h"
#include "Globals.h"
#include "LevelSystem.h"

StateEndChapter::StateEndChapter(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State("endchapter", stateManager, resourceManager)
{

}

StateEndChapter::~StateEndChapter()
{

}

void StateEndChapter::OnEnterState()
{
	m_fadeOutTimer = Constants::Flow::endChapterFadeTimer;
	m_fadingOut = false;
}

void StateEndChapter::OnLeaveState()
{

}

void StateEndChapter::OnPauseState()
{

}

void StateEndChapter::OnResumeState()
{

}

bool StateEndChapter::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	if (m_fadingOut)
	{
		//Wait until fade finished
		if (!Globals::Game::world->IsFading())
		{
			//End of chapter, delete world
			delete Globals::Game::world;
			Globals::Game::world = nullptr;

			//Next level
			LevelSystem::AdvanceLevel();

			//Load next
			m_stateManager.SwapState("loading");
		}

	}
	else
	{
		m_fadeOutTimer -= deltaTime;

		if (m_fadeOutTimer <= 0.0f)
		{
			//Fade out
			Globals::Game::world->BeginFade(-Constants::Flow::defaultFadeSpeed);
			m_fadingOut = true;
		}
	}

	return true;
}

void StateEndChapter::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}