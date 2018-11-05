#pragma once

#include <ion/io/Archive.h>
#include <ion/renderer/Types.h>

class LevelDescriptor;
class Level;
class World;
class Player;
class GameCamera;

namespace Globals
{
	void Serialise(ion::io::Archive& archive);

	namespace Players
	{
		extern Player* player1;
		extern Player* player2;
	}

	namespace Game
	{
		extern int levelIdx;
		extern Level* level;
		extern World* world;
		extern GameCamera* camera;
	}

	namespace Rendering
	{
		extern int windowWidth;
		extern int windowHeight;
		extern int gameCanvasWidth;
		extern int gameCanvasHeight;
	}
}