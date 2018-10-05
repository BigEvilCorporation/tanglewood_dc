///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateLoading.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level loading state
///////////////////////////////////////////////////////////////

#include "StateLoading.h"
#include "Globals.h"
#include "levels/LevelList.h"
#include "framework/World.h"

StateLoading::StateLoading(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State("loading", stateManager, resourceManager)
{

}

StateLoading::~StateLoading()
{

}

void StateLoading::OnEnterState()
{
	//Get level desc
	const LevelDescriptor& levelDesc = Constants::levels[Globals::Game::levelIdx];

	//If world already exists, just reset it
	if (Globals::Game::world)
	{
		//Delete all entities
		Globals::Game::world->DeleteGameObjects();

		//Reset physics world
		Globals::Game::world->GetPhysicsWorld().RemoveAllObjects();

		//If new act
		if (Globals::Game::levelIdx != Globals::Game::world->GetLevelIdx())
		{
			//Load it
			Globals::Game::world->LoadAct(Globals::Game::levelIdx, levelDesc.actName, levelDesc.bgName);
		}
	}
	else
	{
		//Create world
		Globals::Game::world = new World();

		//TODO: Thread this

		//Load sprite data from Beehive project file
		Globals::Game::world->LoadSprites(levelDesc.spriteDataFile);

		//Load first level data file from Beehive project file
		Globals::Game::world->LoadLevel(levelDesc.levelDataFile);

		//Load first act
		Globals::Game::world->LoadAct(Globals::Game::levelIdx, levelDesc.actName, levelDesc.bgName);
	}

	//Create game objects
	Globals::Game::world->CreateGameObjects();
}

void StateLoading::OnLeaveState()
{

}

void StateLoading::OnPauseState()
{

}

void StateLoading::OnResumeState()
{

}

bool StateLoading::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	//Next state
	m_stateManager.SwapState("gameplay");

	return true;
}

void StateLoading::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}