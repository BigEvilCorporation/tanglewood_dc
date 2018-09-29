#pragma once

#include "framework/Animation.h"

namespace Animations
{
	namespace Player
	{
		static const AnimType idle =			{ "idle",			"idle",			AnimFlags::Loop };
		static const AnimType dead =			{ "dead",			"dead",			AnimFlags::FreezeMovementX };
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
	}

	namespace Boulder
	{
		static const AnimType roll =			{ "roll",			"roll",			AnimFlags::Loop };
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
		}
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
