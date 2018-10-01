#include "Globals.h"

namespace Globals
{
	void Serialise(ion::io::archive& archive)
	{
		archive.Serialise(levelIdx, "levelIdx");
		archive.Serialise(screenSize, "screenSize");
	}

	int levelIdx = 0;
	ion::Vector2i screenSize;
}