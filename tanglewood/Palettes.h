#pragma once

#include "framework/PaletteTools.h"
#include "Constants.h"

namespace Assets
{
	namespace Palettes
	{
		extern Palette active[Constants::MegaDrive::maxPalettes];

		namespace World
		{
			extern ion::render::Texture* shared;

			extern Palette day;
			extern Palette dusk;
			extern Palette night;
		}

		namespace Player
		{
			extern ion::render::Texture* shared;

			extern Palette red;
			extern Palette green;
			extern Palette blue;
			extern Palette yellow;
			extern Palette white;
		}
	}
}