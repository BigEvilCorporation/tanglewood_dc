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
#include "Palettes.h"

#include "levels/LevelList.h"

#include "framework/PaletteTools.h"
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
	//if (Globals::Game::world)
	//{
	//	//Delete all entities
	//	Globals::Game::world->DeleteGameObjects();
	//
	//	//Reset physics world
	//	Globals::Game::world->GetPhysicsWorld().RemoveAllObjects();
	//
	//	//If new act
	//	if (Globals::Game::levelIdx != Globals::Game::world->GetLevelIdx())
	//	{
	//		//Load it
	//		Globals::Game::world->LoadAct(Globals::Game::levelIdx, levelDesc.actName, levelDesc.bgName);
	//	}
	//}
	//else
	//{

	if (Globals::Game::world)
	{
		delete Globals::Game::world;
	}

		//Create world
		ion::debug::Log("Creating world");
		Globals::Game::world = new World();

		//TODO: Thread this

		//Load first level data file from Beehive project file
		ion::debug::Log("Loading level");
		Project* project = Globals::Game::world->LoadLevelData(levelDesc.levelDataFile);

		//Load first act
		ion::debug::Log("Loading act");
		Globals::Game::world->LoadAct(*project, Globals::Game::levelIdx, levelDesc.actName, levelDesc.bgName);
	//}

	//TODO: Move to global assets
	ion::debug::Log("Loading palettes");
	LoadGlobalPalettes();

	ion::debug::Log("Loading gameobj types");
	Globals::Game::world->LoadGameObjectTypes(*project, "assets/gameobjectsHD.bee_gameobj");

	//Done with project data
	ion::debug::log << "Mem used before project deletion: " << ion::debug::GetRAMUsed() << ion::debug::end;
	delete project;
	ion::debug::log << "Mem used after project deletion: " << ion::debug::GetRAMUsed() << ion::debug::end;

	//Load sprite data from Beehive project file
	//TODO: Move to global assets
	ion::debug::Log("Loading sprites");
	Globals::Game::world->LoadSprites(levelDesc.spriteDataFile);

	//Create game objects
	ion::debug::Log("Creating game objects");
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

void StateLoading::LoadGlobalPalettes()
{
	if (!Assets::Palettes::Player::shared)
	{
		if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_red"))
		{
			Assets::Palettes::Player::red = *actor->GetMasterPalette();
		}

		if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_green"))
		{
			Assets::Palettes::Player::green = *actor->GetMasterPalette();
		}

		if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_blue"))
		{
			Assets::Palettes::Player::blue = *actor->GetMasterPalette();
		}

		if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_yellow"))
		{
			Assets::Palettes::Player::yellow = *actor->GetMasterPalette();
		}

		if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_white"))
		{
			Assets::Palettes::Player::white = *actor->GetMasterPalette();
		}

		Assets::Palettes::Player::shared = PaletteTools::CreatePaletteTexture(Assets::Palettes::Player::red);
	}
}