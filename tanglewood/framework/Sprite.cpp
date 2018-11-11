///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Sprite.cpp
// Date:		11th November 2018
// Authors:		Matt Phillips
// Description:	Sprite sheet loader
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Sprite.h"

#include "Shaders.h"

const std::vector<ion::render::VertexBuffer::Element> Sprite::s_vertexLayout =
{
	ion::render::VertexBuffer::Element({ ion::render::VertexBuffer::ePosition, ion::render::VertexBuffer::eFloat, 3 }),
	ion::render::VertexBuffer::Element({ ion::render::VertexBuffer::eTexCoord, ion::render::VertexBuffer::eFloat, 2 }),

};

Sprite::Sprite()
{
}

Sprite::Sprite(const Actor& actor)
{
	LoadActor(actor);
}

Sprite::~Sprite()
{
	for (std::map<std::string, Sheet>::iterator it = m_sheets.begin(), end = m_sheets.end(); it != end; it++)
	{
		for (int i = 0; i < it->second.m_frames.size(); i++)
		{
			delete it->second.m_frames[i].material;
			delete it->second.m_frames[i].texture;
		}

		delete it->second.m_primitive;
	}
}

void Sprite::LoadActor(const Actor& actor)
{
	u32 texMemBefore = ion::render::Texture::GetTextureMemoryUsed();
	ion::debug::log << "Loading actor " << actor.GetName()
		<< " : texture memory used: " << texMemBefore
		<< " (" << (float)texMemBefore / 1024.0f << "kb)" << ion::debug::end;

	for (TSpriteSheetMap::const_iterator it = actor.SpriteSheetsBegin(), end = actor.SpriteSheetsEnd(); it != end; ++it)
	{
		LoadSheet(it->second);
	}

	u32 texMemAfter = ion::render::Texture::GetTextureMemoryUsed();
	ion::debug::log << "Loaded actor " << actor.GetName() << " : texture memory used: " << texMemAfter
		<< " (" << (float)texMemAfter / 1024.0f << "kb)"
		<< " cost " << (texMemAfter-texMemBefore) << " (" << (float)(texMemAfter - texMemBefore)/1024.0f << "kb)" << ion::debug::end;
}

void Sprite::LoadSheet(const SpriteSheet& spriteSheet)
{
	const int tileWidth = 8;
	const int tileHeight = 8;

	//Add to map
	Sheet& sheet = m_sheets[spriteSheet.GetName()];

	u32 widthTiles = spriteSheet.GetWidthTiles();
	u32 heightTiles = spriteSheet.GetHeightTiles();
	u32 quadWidth = widthTiles * tileWidth;
	u32 quadHeight = heightTiles * tileHeight;
	u32 textureWidth = ion::maths::NextPowerOfTwo(quadWidth);
	u32 textureHeight = ion::maths::NextPowerOfTwo(quadHeight);
	u32 bytesPerPixel = 4;
	u32 textureSize = textureWidth * textureHeight * bytesPerPixel;

	//Create primitive
	sheet.m_primitive = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2((float)spriteSheet.GetWidthTiles() * (tileWidth / 2.0f), (float)spriteSheet.GetHeightTiles() * (tileHeight / 2.0f)), s_vertexLayout);

	//Set UV coords
	ion::render::TexCoord coords[4];

	const float top = (float)quadHeight / (float)textureHeight;
	const float left = 0.0f;
	const float bottom = 0.0f;
	const float right = (float)quadWidth / (float)textureWidth;

	//Top left
	coords[0].x = left;
	coords[0].y = top;
	//Bottom left
	coords[1].x = left;
	coords[1].y = bottom;
	//Bottom right
	coords[2].x = right;
	coords[2].y = bottom;
	//Top right
	coords[3].x = right;
	coords[3].y = top;

	sheet.m_primitive->SetTexCoords(coords);

	//Create all render frame textures
	for (int i = 0; i < spriteSheet.GetNumFrames(); i++)
	{
		//Create new render frame
		Sheet::Frame renderFrame;

		//Create tileset texture for frame
		renderFrame.texture = ion::render::Texture::Create(textureWidth, textureHeight);

		//Create material
		renderFrame.material = new ion::render::Material();
		renderFrame.material->AddDiffuseMap(renderFrame.texture);
		renderFrame.material->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f, 1.0f));

#if defined ION_RENDERER_SHADER
#if USE_PALETTE_TEXTURES
		renderFrame.material->SetVertexShader(Assets::Shaders::IndexTexture::vertexShader.Get());
		renderFrame.material->SetPixelShader(Assets::Shaders::IndexTexture::pixelShader.Get());
#else
		renderFrame.material->SetVertexShader(Assets::Shaders::FlatTextured::vertexShader.Get());
		renderFrame.material->SetPixelShader(Assets::Shaders::FlatTextured::pixelShader.Get());
#endif
#endif

		//Insert frame
		sheet.m_frames.push_back(renderFrame);

		//Enumerate anims
		for (TSpriteAnimMap::const_iterator it = spriteSheet.AnimationsBegin(), end = spriteSheet.AnimationsEnd(); it != end; ++it)
		{
			sheet.m_animations[it->second.GetName()] = &it->second;
		}
	}

	//Paint sprite sheet
	PaintSheet(spriteSheet, spriteSheet.GetPalette());
}

void Sprite::PaintSheet(const SpriteSheet& spriteSheet, const Palette& palette)
{
	Sheet& sheet = m_sheets[spriteSheet.GetName()];

	const int tileWidth = 8;
	const int tileHeight = 8;
	u32 widthTiles = spriteSheet.GetWidthTiles();
	u32 heightTiles = spriteSheet.GetHeightTiles();

	for (int i = 0; i < spriteSheet.GetNumFrames(); i++)
	{
		//Get spriteSheet frame
		const SpriteSheetFrame& spriteSheetFrame = spriteSheet.GetFrame(i);

		//Get render frame
		Sheet::Frame& renderFrame = sheet.m_frames[i];

#if USE_PALETTE_TEXTURES
		u32 bytesPerPixel = 1;
#else
		u32 bytesPerPixel = 4;
#endif

		u32 textureWidth = renderFrame.texture->GetWidth();
		u32 textureHeight = renderFrame.texture->GetHeight();
		u32 textureSize = textureWidth * textureHeight * bytesPerPixel;

		u8* data = new u8[textureSize];
		ion::memory::MemSet(data, 0, textureSize);

		for (int tileX = 0; tileX < spriteSheet.GetWidthTiles(); tileX++)
		{
			for (int tileY = 0; tileY < spriteSheet.GetHeightTiles(); tileY++)
			{
				//Genesis spriteSheet order = column major
				const Tile& tile = spriteSheetFrame[(tileX * heightTiles) + tileY];

				//Invert Y for OpenGL
				int tileY_inv = spriteSheet.GetHeightTiles() - 1 - tileY;

				//Paint tile to texture
				for (int pixelY = 0; pixelY < tileHeight; pixelY++)
				{
					for (int pixelX = 0; pixelX < tileWidth; pixelX++)
					{
						//Invert Y for OpenGL
						int pixelY_OGL = tileHeight - 1 - pixelY;

						u8 colourIdx = tile.GetPixelColour(pixelX, pixelY_OGL);

						int destPixelX = (tileX * tileWidth) + pixelX;
						int destPixelY = (tileY_inv * tileHeight) + pixelY;
						u32 pixelIdx = (destPixelY * textureWidth) + destPixelX;
						u32 dataOffset = pixelIdx * bytesPerPixel;
						ion::debug::Assert(dataOffset + bytesPerPixel <= textureSize, "eOut of bounds");

#if USE_PALETTE_TEXTURES
						data[dataOffset] = colourIdx;
#else
						const Colour& colour = palette.GetColour(colourIdx);
						data[dataOffset] = colour.GetRed();
						data[dataOffset + 1] = colour.GetGreen();
						data[dataOffset + 2] = colour.GetBlue();
						data[dataOffset + 3] = colourIdx > 0 ? 255 : 0;
#endif
					}
				}
			}
		}

#if USE_PALETTE_TEXTURES
		ion::render::Texture::Format format = ion::render::Texture::Format::R;
		ion::render::Texture::BitsPerPixel bpp = ion::render::Texture::BitsPerPixel::BPP8;
#else
		ion::render::Texture::Format format = ion::render::Texture::Format::RGBA;
		ion::render::Texture::BitsPerPixel bpp = ion::render::Texture::BitsPerPixel::BPP24;
#endif

		//Load texture data
		renderFrame.texture->Load(textureWidth, textureHeight, format, format, bpp, false, false, data);

		//Reset filter
		renderFrame.texture->SetMinifyFilter(ion::render::Texture::Filter::Nearest);
		renderFrame.texture->SetMagnifyFilter(ion::render::Texture::Filter::Nearest);
		renderFrame.texture->SetWrapping(ion::render::Texture::Wrapping::Clamp);

		delete[] data;
	}
}

const Sprite::Sheet* Sprite::FindSheet(const std::string& name) const
{
	const Sprite::Sheet* sheet = nullptr;

	std::map<std::string, Sheet>::const_iterator it = m_sheets.find(name);
	if (it != m_sheets.end())
	{
		sheet = &it->second;
	}

	return sheet;
}