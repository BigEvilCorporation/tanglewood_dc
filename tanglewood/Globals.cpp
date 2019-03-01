#include "Globals.h"
#include "levels/LevelList.h"
#include "framework/Camera.h"
#include "tanglewood/PlayerController.h"

namespace Globals
{
	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(Game::levelIdx, "Game::levelIdx");
	}

	namespace Players
	{
		Player* player1 = nullptr;
		Player* player2 = nullptr;
		PlayerController* playerController1 = nullptr;
	}

	namespace Game
	{
		int levelIdx = (int)Constants::Levels::l1a1_Harlequin_Act1;
		World* world = nullptr;
		Level* level = nullptr;
		GameCamera* camera = nullptr;
	}

	namespace Flow
	{
		float levelTransitionVelX = 0.0f;
	}

	namespace Rendering
	{
		int windowWidth = 1024;
		int windowHeight = 768;
		int gameCanvasWidth = 320;
		int gameCanvasHeight = 240;
	}
}