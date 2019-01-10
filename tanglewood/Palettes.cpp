#include "Palettes.h"

namespace Assets
{
	namespace Palettes
	{
		Palette active[Constants::MegaDrive::maxPalettes];

		namespace World
		{
			ion::render::Texture* shared = nullptr;

			Palette day;
			Palette dusk;
			Palette night;
		}

		namespace Player
		{
			ion::render::Texture* shared = nullptr;

			Palette red;
			Palette green;
			Palette blue;
			Palette yellow;
			Palette white;
		}
	}
}