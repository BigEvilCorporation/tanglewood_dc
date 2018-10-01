#include "Globals.h"

namespace Globals
{
	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(World::levelIdx, "World::levelIdx");
	}

	namespace World
	{
		int levelIdx = 0;
	}
}