#include "Globals.h"
#include "levels/LevelList.h"
#include "framework/Camera.h"

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
	}

	namespace Game
	{
		int levelIdx = (int)Constants::Levels::l1a2_Harlequin_Act2;
		World* world = nullptr;
		Level* level = nullptr;
		GameCamera* camera = nullptr;
	}

	namespace Rendering
	{
		int windowWidth = 1024;
		int windowHeight = 760;
		int gameCanvasWidth = 320;
		int gameCanvasHeight = 240;
	}
}