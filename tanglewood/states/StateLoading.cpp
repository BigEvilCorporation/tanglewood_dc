///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		StateLoading.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level loading state
///////////////////////////////////////////////////////////////

#include "StateLoading.h"
#include "Constants.h"
#include "Globals.h"
#include "Palettes.h"
#include "Animations.h"

#include "levels/LevelList.h"

#include "framework/PaletteTools.h"
#include "framework/World.h"

#include <ion/core/thread/Sleep.h>

//HACK
#include <ion/renderer/opengl/OpenGLInclude.h>

StateLoading::StateLoading(ion::gamekit::StateManager& stateManager, ion::io::ResourceManager& resourceManager)
	: ion::gamekit::State("loading", stateManager, resourceManager)
{
	m_loadingSprite = nullptr;
}

StateLoading::~StateLoading()
{

}

void StateLoading::SetCameraPosition(const ion::Vector2& position)
{
	//Calc ratio of window to screen size
	ion::Vector3 cameraZoom;
	cameraZoom.x = (float)Globals::Rendering::windowWidth / (float)Globals::Rendering::gameCanvasWidth;
	cameraZoom.y = (float)Globals::Rendering::windowHeight / (float)Globals::Rendering::gameCanvasHeight;
	cameraZoom.z = 1.0f;

	//Set camera zoom
	m_loadingCamera.SetZoom(cameraZoom);

	//Compensate camera pos
	ion::Vector3 cameraPos;
	cameraPos.x = position.x - (float)Globals::Rendering::gameCanvasWidth / 2.0f;
	cameraPos.y = position.y - (float)Globals::Rendering::gameCanvasHeight / 2.0f;
	cameraPos.z = -0.1f;

	//Set camera pos
	m_loadingCamera.SetPosition(cameraPos);
}

void StateLoading::OnEnterState()
{
	//Reset fader
	m_fader.ResetToBlack();
	m_fadingOut = false;

	//Start loading thread
	m_loadingThread = new LoadingThread();

#if THREADED_LOADING
	m_loadingThread->Run();

	//Load loading screen assets
	ion::io::File file("assets/loading.bee_sprites", ion::io::File::eOpenRead);
	if (file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.Serialise(m_actors, "actors");

		if (m_actors.size() > 0 && m_actors.begin()->second.GetSpriteSheetCount() > 0)
		{
			//Create game obj
			Actor& actor = m_actors.begin()->second;
			const SpriteSheet& sheet = actor.SpriteSheetsBegin()->second;

			m_loadingWorld = new World();
			m_loadingGameObjType = new GameObjectType(0);
			m_loadingGameObj = new GameObject(0, 0, ion::Vector2i(0, 0), ion::Vector2i(sheet.GetWidthTiles() * Constants::MegaDrive::tileWidth, sheet.GetHeightTiles() * Constants::MegaDrive::tileHeight));
			m_loadingWorld->LoadSprite("nymn");
			m_loadingSprite = new SpriteObj(*m_loadingWorld, *m_loadingGameObj, *m_loadingGameObjType, &actor);

			//Setup rendering
			m_loadingScreenSize.x = Globals::Rendering::gameCanvasWidth;
			m_loadingScreenSize.y = Globals::Rendering::gameCanvasHeight;
			ion::Vector2 centre = m_loadingSprite->GetWorldCentre();

			SetCameraPosition(ion::Vector2(
				centre.x - (m_loadingScreenSize.x / 2) + (m_loadingSprite->m_size.x / 2) + 8,
				m_loadingScreenSize.y - centre.y + (m_loadingScreenSize.y / 2) - (m_loadingSprite->m_size.y / 2) - 8));

			//Begin animation
			m_loadingSprite->PlayAnimation(Animations::Loading::run);

			//Begin fading up
			m_fader.BeginFade(Constants::Flow::defaultFadeSpeed);
		}
	}
#else
	m_loadingThread->Entry();
#endif
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
	//Update loading sprite
	if (m_loadingSprite)
	{
		m_loadingSprite->Update(deltaTime);
	}

	//Update fader
	m_fader.Update(deltaTime);

	//Wait until loading thread done
	if (m_loadingThread->m_running)
	{
		//Allow loading thread most of the CPU time
		ion::thread::Sleep(25);
	}
	else
	{
		if (m_fadingOut)
		{
			//Wait for fade
			if (!m_fader.IsFading())
			{
				//Done with loading thread
#if THREADED_LOADING
				if (m_loadingThread)
				{
					delete m_loadingThread;
					m_loadingThread = nullptr;
				}
#endif

				//Done with loading world

				//Done with loading assets

				//Next state
				m_stateManager.SwapState("gameplay");
			}
		}
		else
		{
			//Begin fade out
			m_fader.BeginFade(-Constants::Flow::defaultFadeSpeed);
			m_fadingOut = true;
		}
	}

	return true;
}

void StateLoading::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{
	//Render loading sprite
	if (m_loadingSprite)
	{
		m_loadingSprite->Render(renderer, m_loadingCamera, viewport, m_loadingCamera.GetTransform().GetInverse(), m_loadingScreenSize);
	}

	//Render fader
	m_fader.Render(renderer);
}

void StateLoading::LoadingThread::Entry()
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

		//Begin fade up
		Globals::Game::world->BeginFade(Constants::Flow::defaultFadeSpeed);

		////////////////////////////////////////////////////////////////////////////
		// GLOBAL DATA
		// TODO: MOVE TO GLOBAL LOAD STATE
		////////////////////////////////////////////////////////////////////////////

		//Load game objects types
		ion::debug::Log("Loading gameobj types");
		Globals::Game::world->LoadGameObjectTypes("assets/gameobjtypes.bee");

		//Load global palettes
		#if USE_PALETTES
		ion::debug::Log("Loading palettes");
		LoadGlobalPalettes();
		#endif

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		//Load per-chapter data
		ion::debug::Log("Loading chapter data");
		Globals::Game::world->LoadChapterData(levelDesc);

		//Load per-act data
		ion::debug::Log("Loading act data");
		Globals::Game::world->LoadActData(levelDesc);
	}

	//Load sprite data
	ion::debug::Log("Loading sprites");
	Globals::Game::world->LoadSprites();

	//Create game objects
	ion::debug::Log("Creating game objects");
	Globals::Game::world->CreateGameObjects();

	//Configure camera
	//ion::debug::Log("Init camera");
	//Globals::Game::world->InitCamera();

	//Pre-stream map
	ion::debug::Log("Pre-streaming map");
	Globals::Game::world->PreStreamMap();

	//Done
	ion::debug::Log("Loading complete");
	m_running = false;
}

void StateLoading::LoadingThread::LoadGlobalPalettes()
{
	if (!Assets::Palettes::Player::shared)
	{
		if (Globals::Game::world->LoadSprite("nymn_pal_red"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_red"))
			{
				Assets::Palettes::Player::red = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("nymn_pal_green"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_green"))
			{
				Assets::Palettes::Player::green = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("nymn_pal_blue"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_blue"))
			{
				Assets::Palettes::Player::blue = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("nymn_pal_yellow"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_yellow"))
			{
				Assets::Palettes::Player::yellow = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("nymn_pal_white"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_white"))
			{
				Assets::Palettes::Player::white = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("nymn_pal_white"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("nymn_pal_white"))
			{
				Assets::Palettes::Player::white = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("fuzzl"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("fuzzl"))
			{
				Assets::Palettes::Fuzzl::fuzzl = *actor->GetMasterPalette();
			}
		}

		if (Globals::Game::world->LoadSprite("djakk"))
		{
			if (const Actor* actor = Globals::Game::world->FindActor("djakk"))
			{
				Assets::Palettes::Djakk::djakk = *actor->GetMasterPalette();
			}
		}

#if USE_PALETTE_TEXTURES
		ion::debug::Log("Creating shared palette textures");
		Assets::Palettes::Player::shared = PaletteTools::CreatePaletteTexture(Assets::Palettes::Player::red);
		Assets::Palettes::Fuzzl::shared = PaletteTools::CreatePaletteTexture(Assets::Palettes::Fuzzl::fuzzl);
		Assets::Palettes::Djakk::shared = PaletteTools::CreatePaletteTexture(Assets::Palettes::Djakk::djakk);
#endif
	}
}