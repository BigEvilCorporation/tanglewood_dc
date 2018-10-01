#pragma once

#include <ion/io/Archive.h>

namespace Globals
{
	void Serialise(ion::io::archive& archive);

	extern int levelIdx;
	extern ion::Vector2i screenSize;
}