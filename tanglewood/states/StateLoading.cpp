///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateLoading.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level loading state
///////////////////////////////////////////////////////////////

#include "StateLoading.h"

StateLoading::StateLoading(World& world, const LevelDescriptor& levelDesc, StateGameplay& stateGameplay, ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State(stateManager, resourceManager)
	, m_world(world)
	, m_levelDesc(levelDesc)
	, m_stateGameplay(stateGameplay)
{

}

StateLoading::~StateLoading()
{

}

void StateLoading::OnEnterState()
{
	//TODO: Thread this

	//Load sprite data from Beehive project file
	m_world.LoadSprites(m_levelDesc.spriteDataFile);

	//Load first level data file from Beehive project file
	m_world.LoadLevel(m_levelDesc.levelDataFile);

	//Load first act
	m_world.LoadAct(m_levelDesc.actName, m_levelDesc.bgName);

	//Create game objects
	m_world.CreateGameObjects();
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
	m_stateManager.SwapState((ion::gamekit::State&)m_stateGameplay);

	return true;
}

void StateLoading::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}