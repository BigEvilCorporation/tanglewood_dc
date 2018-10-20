#include "Globals.h"
#include "levels/LevelList.h"

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
		int levelIdx = (int)Constants::Levels::l1a1_Harlequin_Act1;
		World* world = nullptr;
		Level* level = nullptr;
		ion::render::Camera* camera = nullptr;
	}

	namespace Rendering
	{
		int windowWidth = 1024;
		int windowHeight = 760;
		int gameCanvasWidth = 320;
		int gameCanvasHeight = 240;
	}
}