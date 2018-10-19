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
			Globals::Game::world->LoadActData(levelDesc);
		}
	}
	else
	{
		//Create world
		ion::debug::Log("Creating world");
		Globals::Game::world = new World();

		////////////////////////////////////////////////////////////////////////////
		// GLOBAL DATA
		// TODO: MOVE TO GLOBAL LOAD STATE
		////////////////////////////////////////////////////////////////////////////

		//Load game objects types
		ion::debug::Log("Loading gameobj types");
		Globals::Game::world->LoadGameObjectTypes("assets/gameobjtypes.bee");

		//Load sprite data from Beehive project file
		ion::debug::Log("Loading sprites");
		Globals::Game::world->LoadSprites("assets/sprites.bee_sprites");

		//Load global palettes
		ion::debug::Log("Loading palettes");
		LoadGlobalPalettes();

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		//TODO: Thread this

		//Load per-chapter data
		ion::debug::Log("Loading level");
		Globals::Game::world->LoadChapterData(levelDesc);

		//Load per-act data
		ion::debug::Log("Loading act");
		Globals::Game::world->LoadActData(levelDesc);
	}

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