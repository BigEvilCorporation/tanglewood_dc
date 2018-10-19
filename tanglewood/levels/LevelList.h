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
		////////////////////////////////////////////////////////
		// CHAPTER 1 - HARLEQUIN FOREST
		////////////////////////////////////////////////////////
		{
			"assets/levels/l1/tileset.bee",
			"assets/levels/l1/stamps.bee",
			"assets/levels/l1/palettes.bee",
			"assets/levels/l1/terraintileset.bee",
			"assets/levels/l1/l1a1/collisionmap.bee",
			"assets/levels/l1/l1a1/gameobjects.bee",
			"assets/levels/l1/l1a1/stampmap.bee",
			"assets/levels/l1/l1bg/stampmap.bee",
			[]() { return new L1A1(); }
		},

		{
			"assets/levels/l1/tileset.bee",
			"assets/levels/l1/stamps.bee",
			"assets/levels/l1/palettes.bee",
			"assets/levels/l1/terraintileset.bee",
			"assets/levels/l1/l1a2/collisionmap.bee",
			"assets/levels/l1/l1a2/gameobjects.bee",
			"assets/levels/l1/l1a2/stampmap.bee",
			"assets/levels/l1/l1bg/stampmap.bee",
			[]() { return new L1A2(); }
		},

		{
			"assets/levels/l1/tileset.bee",
			"assets/levels/l1/stamps.bee",
			"assets/levels/l1/palettes.bee",
			"assets/levels/l1/terraintileset.bee",
			"assets/levels/l1/l1a3/collisionmap.bee",
			"assets/levels/l1/l1a3/gameobjects.bee",
			"assets/levels/l1/l1a3/stampmap.bee",
			"assets/levels/l1/l1bg/stampmap.bee",
			[]() { return new L1A3(); }
		},

		{
			"assets/levels/l1/tileset.bee",
			"assets/levels/l1/stamps.bee",
			"assets/levels/l1/palettes.bee",
			"assets/levels/l1/terraintileset.bee",
			"assets/levels/l1/l1a4/collisionmap.bee",
			"assets/levels/l1/l1a4/gameobjects.bee",
			"assets/levels/l1/l1a4/stampmap.bee",
			"assets/levels/l1/l1bg/stampmap.bee",
			[]() { return new L1A4(); }
		},

		////////////////////////////////////////////////////////
		// CHAPTER 2 - STORM WARNING
		////////////////////////////////////////////////////////
		{
			"assets/levels/l2/tileset.bee",
			"assets/levels/l2/stamps.bee",
			"assets/levels/l2/palettes.bee",
			"assets/levels/l2/terraintileset.bee",
			"assets/levels/l2/l2a1/collisionmap.bee",
			"assets/levels/l2/l2a1/gameobjects.bee",
			"assets/levels/l2/l2a1/stampmap.bee",
			"assets/levels/l2/l2bg/stampmap.bee",
			[]() { return new L1A1(); }
		},
	};

	enum class Levels
	{
		l1a1_Harlequin_Act1,
		l1a1_Harlequin_Act2,
		l1a1_Harlequin_Act3,
		l1a1_Harlequin_Act4,
		l2a1_StormWarning_Act1,
	};
}
