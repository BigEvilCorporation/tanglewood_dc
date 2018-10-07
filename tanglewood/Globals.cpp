#include "Globals.h"

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
		int levelIdx = 2;
		World* world = nullptr;
		Level* level = nullptr;
		ion::render::Camera* camera = nullptr;
	}

	namespace Rendering
	{
		int windowWidth = 320;
		int windowHeight = 240;
	}
}