///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateLoading.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level loading state
///////////////////////////////////////////////////////////////

#include "StateLoading.h"

StateLoading::StateLoading(World& world, Level& level, StateGameplay& stateGameplay, ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State(stateManager, resourceManager)
	, m_world(world)
	, m_level(level)
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
	m_world.LoadSprites(m_level.m_levelData.spriteDataFile);

	//Load first level data file from Beehive project file
	m_world.LoadLevel(m_level.m_levelData.levelDataFile);

	//Load first act
	m_world.LoadAct(m_level.m_levelData.actName, m_level.m_levelData.bgName);

	//Create game objects
	m_world.CreateGameObjects();

	//Next state
	m_stateManager.SwapState((ion::gamekit::State&)m_stateGameplay);
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
	return false;
}

void StateLoading::Render(ion::render::Renderer& renderer, ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}