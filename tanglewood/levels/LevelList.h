#pragma once

#include "framework/Level.h"

#include "L1A1.h"
#include "L1A2.h"
#include "L1A3.h"
#include "L1A4.h"

namespace Constants
{
	static const LevelDescriptor levels[] =
	{
		{ "assets/sprites.bee_sprites", "assets/l1.bee", "l1a1", "l1bg", []() { return new L1A1(); } },
		{ "assets/sprites.bee_sprites", "assets/l1.bee", "l1a2", "l1bg", []() { return new L1A2(); } },
		{ "assets/sprites.bee_sprites", "assets/l1.bee", "l1a3", "l1bg", []() { return new L1A3(); } },
		{ "assets/sprites.bee_sprites", "assets/l1.bee", "l1a4", "l1bg", []() { return new L1A4(); } },

		{ "assets/sprites.bee_sprites", "assets/l2.bee", "l2a1", "l2bg", []() { return new L1A1(); } },
	};

	enum class Levels
	{
		l1a1_Harlequin_Act1,
		l1a1_Harlequin_Act2,
		l1a1_Harlequin_Act3,
		l1a1_Harlequin_Act4,
	};
}
