#pragma once

#include <ion/io/Archive.h>
#include <ion/renderer/Camera.h>

class LevelDescriptor;
class Level;
class World;
class Player;

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
		extern ion::render::Camera* camera;
	}
}