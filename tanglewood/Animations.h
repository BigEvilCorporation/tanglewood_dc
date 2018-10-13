#pragma once

#include "framework/Animation.h"

namespace Animations
{
	namespace Player
	{
		static const AnimType idle =			{ "idle",			"idle",			AnimFlags::Loop };
		static const AnimType dead =			{ "death",			"death",		AnimFlags::FreezeMovementX };
		static const AnimType run =				{ "run",			"run",			AnimFlags::Loop };
		static const AnimType walk =			{ "walk",			"walk",			AnimFlags::Loop };
		static const AnimType jump =			{ "jump",			"jump",			AnimFlags::Loop };
		static const AnimType glide =			{ "glide",			"glide",		AnimFlags::Loop };
		static const AnimType push =			{ "push",			"push",			AnimFlags::Loop };
		static const AnimType pushHeavy =		{ "pushheavy",		"pushheavy",	AnimFlags::Loop };
		static const AnimType fall =			{ "fall",			"fall",			AnimFlags::Loop };
		static const AnimType walktorun =		{ "walktorun",		"walktorun",	0 };
		static const AnimType waterWade =		{ "waterWade",		"waterWade",	AnimFlags::Loop };
		static const AnimType colourSwitch =	{ "colourswitch",	"colourswitch",	AnimFlags::FreezeMovementX };
		static const AnimType mount =			{ "mountdjakk",		"mountdjakk",	AnimFlags::FreezeMovementX };
		static const AnimType rideTrot =		{ "ridedjakk",		"ridedjakk",	AnimFlags::Loop };
		static const AnimType rideGallop =		{ "ridedjakkrun",	"ridedjakkrun",	AnimFlags::Loop };
		static const AnimType rideJump =		{ "ridedjakkjump",	"ridedjakkjump",AnimFlags::Loop };
		static const AnimType laydown =			{ "laydown",		"laydown",		AnimFlags::FreezeMovementX };
		static const AnimType sleep =			{ "sleep",			"sleep",		AnimFlags::FreezeMovementX | AnimFlags::Loop };
	}

	namespace Boulder
	{
		static const AnimType roll =			{ "roll",			"roll",			AnimFlags::Loop };
		static const AnimType crack =			{ "crack",			"crack",		AnimFlags::FreezeMovementX };
	}

	namespace Djakk
	{
		static const AnimType idle =			{ "idle",			"idle",			AnimFlags::Loop };
		static const AnimType roar =			{ "roar",			"roar",			AnimFlags::FreezeMovementX };
		static const AnimType bite =			{ "bite",			"bite",			AnimFlags::FreezeMovementX };
		static const AnimType dead =			{ "death",			"death",		AnimFlags::FreezeMovementX };
		static const AnimType run =				{ "run",			"run",			AnimFlags::Loop };
		static const AnimType walk =			{ "walk",			"walk",			AnimFlags::Loop };
		static const AnimType jump =			{ "jump",			"jump",			AnimFlags::Loop };
		static const AnimType buck =			{ "throw",			"throw",		AnimFlags::FreezeMovementX };
	}

	namespace Firefly
	{
		static const AnimType idle =			{ "red",			"idle",			AnimFlags::Loop };
	}

	namespace Fuzzl
	{
		namespace Yellow
		{
			static const AnimType idle =		{ "yellow_idle",	"yellow_idle",	AnimFlags::Loop };
			static const AnimType watch =		{ "yellow_eyes",	"yellow_eyes",	0 };
			static const AnimType roll =		{ "yellow_roll",	"yellow_roll",	AnimFlags::Loop };
			static const AnimType sleep =		{ "yellow_sleep",	"yellow_sleep",	AnimFlags::Loop };
		}
	}

	namespace Hogg
	{
		static const AnimType idle =			{ "run",			"idle",			AnimFlags::Loop };
		static const AnimType dead =			{ "dead",			"dead",			AnimFlags::FreezeMovementX };
		static const AnimType run =				{ "run",			"run",			AnimFlags::Loop };
		static const AnimType wall =			{ "wall",			"hit",			AnimFlags::FreezeMovementX };
		static const AnimType stuck =			{ "wall",			"stuck",		AnimFlags::Loop };
		static const AnimType unstuck =			{ "wall",			"recover",		AnimFlags::FreezeMovementX };
	}

	namespace Mushroom
	{
		namespace Orient180
		{
			static const AnimType idle =		{ "mushroom_180",	"idle",			AnimFlags::Loop };
			static const AnimType bounce =		{ "mushroom_180",	"bounce",		0 };
		}
	}
}
