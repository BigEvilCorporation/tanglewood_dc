#pragma once

#include <ion/renderer/Texture.h>
#include <ion/beehive/Palette.h>

namespace PaletteTools
{
	ion::render::Texture* CreatePaletteTexture(const Palette& palette);
	void WritePaletteTexture(const Palette& palette, ion::render::Texture* texture);
	void BlendPalettes(const Palette& paletteA, const Palette& paletteB, Palette& paletteOut, float blend);
	void GrayScalePalette(const Palette& palette, Palette& paletteOut);
}