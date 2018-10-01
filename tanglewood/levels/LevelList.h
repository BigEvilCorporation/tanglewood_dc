#pragma once

#include "framework/Level.h"

#include "L1A1.h"
#include "L1A2.h"

namespace Constants
{
	static const LevelDescriptor levels[] =
	{
		{ "cd/sprites.bee_sprites", "cd/l1.bee", "l1a1", "l1bg", []() { return new L1A1(); } },
		{ "cd/sprites.bee_sprites", "cd/l1.bee", "l1a2", "l1bg", []() { return new L1A2(); } },
	};
}
