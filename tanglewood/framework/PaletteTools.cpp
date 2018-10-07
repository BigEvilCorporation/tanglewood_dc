#include "PaletteTools.h"

namespace PaletteTools
{
	ion::render::Texture* CreatePaletteTexture(const Palette& palette)
	{
		u8 paletteData[Palette::coloursPerPalette * 4];
		u8* paletteWritePtr = paletteData;

		for (int i = 0; i < Palette::coloursPerPalette; i++)
		{
			if (palette.IsColourUsed(i))
			{
				*paletteWritePtr++ = palette.GetColour(i).GetRed();
				*paletteWritePtr++ = palette.GetColour(i).GetGreen();
				*paletteWritePtr++ = palette.GetColour(i).GetBlue();
				*paletteWritePtr++ = i > 0 ? 255 : 0;
			}
		}

		ion::render::Texture* texture = ion::render::Texture::Create(Palette::coloursPerPalette, 1, ion::render::Texture::eRGBA, ion::render::Texture::eRGBA, ion::render::Texture::eBPP24, false, false, paletteData);
		texture->SetMinifyFilter(ion::render::Texture::eFilterNearest);
		texture->SetMagnifyFilter(ion::render::Texture::eFilterNearest);

		return texture;
	}

	void WritePaletteTexture(const Palette& palette, ion::render::Texture* texture)
	{
		u8 paletteData[Palette::coloursPerPalette * 4];
		u8* paletteWritePtr = paletteData;

		for (int i = 0; i < Palette::coloursPerPalette; i++)
		{
			if (palette.IsColourUsed(i))
			{
				*paletteWritePtr++ = palette.GetColour(i).GetRed();
				*paletteWritePtr++ = palette.GetColour(i).GetGreen();
				*paletteWritePtr++ = palette.GetColour(i).GetBlue();
				*paletteWritePtr++ = i > 0 ? 255 : 0;
			}
		}

		texture->SetPixels(ion::render::Texture::eRGBA, false, paletteData);
	}

	void BlendPalettes(const Palette& paletteA, const Palette& paletteB, Palette& paletteOut, float blend)
	{
		for (int i = 0; i < Palette::coloursPerPalette; i++)
		{
			if (paletteA.IsColourUsed(i) && paletteB.IsColourUsed(i))
			{
				const Colour& colourA = paletteA.GetColour(i);
				const Colour& colourB = paletteB.GetColour(i);

				Colour colourOut;
				colourOut.SetRed((u8)ion::maths::Lerp((float)colourA.GetRed(), (float)colourB.GetRed(), blend));
				colourOut.SetGreen((u8)ion::maths::Lerp((float)colourA.GetGreen(), (float)colourB.GetGreen(), blend));
				colourOut.SetBlue((u8)ion::maths::Lerp((float)colourA.GetBlue(), (float)colourB.GetBlue(), blend));

				paletteOut.SetColour(i, colourOut);
			}
		}
	}

	void GrayScalePalette(const Palette& palette, Palette& paletteOut)
	{
		for (int i = 0; i < Palette::coloursPerPalette; i++)
		{
			if (palette.IsColourUsed(i))
			{
				const Colour& colour = palette.GetColour(i);

				Colour colourOut;
				u8 grey = (colour.GetRed() + colour.GetGreen() + colour.GetBlue()) / 3;
				colourOut.SetRed(grey);
				colourOut.SetGreen(grey);
				colourOut.SetBlue(grey);

				paletteOut.SetColour(i, colourOut);
			}
		}
	}
}