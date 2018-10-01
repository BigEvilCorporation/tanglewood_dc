#pragma once

#include <ion/io/Archive.h>

namespace Globals
{
	void Serialise(ion::io::Archive& archive);

	namespace World
	{
		extern int levelIdx;
	}
}