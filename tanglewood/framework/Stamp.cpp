///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Stamp.cpp
// Date:		13th January 2017
// Authors:		Matt Phillips
// Description:	Stamp loading/rendering from Beehive data
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////////

#include <ion/core/memory/Memory.h>
#include <ion/maths/Geometry.h>

#include "Stamp.h"
#include "Constants.h"
#include "Globals.h"

#if defined ION_RENDERER_SHADER
#include "Shaders.h"
#endif

StampRenderer::StampRenderer(const Stamp& stamp, const Tileset& tileset, const Palette& palette)
{
#if USE_PALETTE_TEXTURES
	m_paletteTexture = nullptr;
#endif

	const int tileWidth = 8;
	const int tileHeight = 8;

	u32 widthTiles = stamp.GetWidth();
	u32 heightTiles = stamp.GetHeight();
	u32 quadWidth = widthTiles * tileWidth;
	u32 quadHeight = heightTiles * tileHeight;
	u32 textureWidth = ion::maths::NextPowerOfTwo(quadWidth);
	u32 textureHeight = ion::maths::NextPowerOfTwo(quadHeight);
	u32 bytesPerPixel = 4;
	u32 textureSize = textureWidth * textureHeight * bytesPerPixel;

	m_size.x = widthTiles * tileWidth;
	m_size.y = heightTiles * tileHeight;
	m_planeSize.x = (float)widthTiles * (tileWidth / 2.0f);
	m_planeSize.y = (float)heightTiles * (tileHeight / 2.0f);
	m_planePriority = (stamp.GetTileFlags(0, 0) & Map::eHighPlane) ? PlanePriority::PlaneAHigh : PlanePriority::PlaneALow;

	//Create primitive
	m_primitive = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2(1.0f, 1.0f));

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

	m_primitive->SetTexCoords(coords);

	//Create tileset texture for frame
	m_texture = ion::render::Texture::Create(textureWidth, textureHeight);

	u8* data = new u8[textureSize];
	ion::memory::MemSet(data, 0, textureSize);

	for(int tileX = 0; tileX < widthTiles; tileX++)
	{
		for(int tileY = 0; tileY < heightTiles; tileY++)
		{
			//Genesis plane order = row major
			TileId tileId = stamp.GetTile(tileX, tileY);
			const Tile* tile = tileset.GetTile(tileId);

			//Invert Y for OpenGL
			int tileY_inv = heightTiles - 1 - tileY;

			//Get flip flags
			u32 flipFlags = stamp.GetTileFlags(tileX, tileY);

			//Paint tile to texture
			for(int pixelY = 0; pixelY < tileHeight; pixelY++)
			{
				for(int pixelX = 0; pixelX < tileWidth; pixelX++)
				{
					int sourceX = (flipFlags & Map::eFlipX) ? (tileWidth - 1 - pixelX) : pixelX;
					int sourceY = (flipFlags & Map::eFlipY) ? (tileHeight - 1 - pixelY) : pixelY;

					//Invert Y for OpenGL
					int pixelY_OGL = tileHeight - 1 - sourceY;

					u8 colourIdx = tile->GetPixelColour(sourceX, pixelY_OGL);

					int destPixelX = (tileX * tileWidth) + pixelX;
					int destPixelY = (tileY_inv * tileHeight) + pixelY;
					u32 pixelIdx = (destPixelY * textureWidth) + destPixelX;
					u32 dataOffset = pixelIdx * bytesPerPixel;
					ion::debug::Assert(dataOffset + 2 < textureSize, "eOut of bounds");

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

	m_texture->Load(textureWidth, textureHeight, ion::render::Texture::eRGBA, ion::render::Texture::eRGBA, ion::render::Texture::eBPP24, false, false, data);
	m_texture->SetMinifyFilter(ion::render::Texture::eFilterNearest);
	m_texture->SetMagnifyFilter(ion::render::Texture::eFilterNearest);
	m_texture->SetWrapping(ion::render::Texture::eWrapClamp);

	//Create material
	m_material = new ion::render::Material();
	m_material->AddDiffuseMap(m_texture);
	m_material->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f));

#if defined ION_RENDERER_SHADER
#if USE_PALETTE_TEXTURES
	m_material->SetVertexShader(Assets::Shaders::IndexTexture::vertexShader.Get());
	m_material->SetPixelShader(Assets::Shaders::IndexTexture::pixelShader.Get());
#else
	m_material->SetVertexShader(Assets::Shaders::FlatTextured::vertexShader.Get());
	m_material->SetPixelShader(Assets::Shaders::FlatTextured::pixelShader.Get());
#endif
#endif
}

void StampRenderer::Render(ion::render::Renderer& renderer, const ion::Vector2& position, const Bounds& cameraBounds, const ion::Matrix4& cameraInv, bool flippedX, bool flippedY)
{
	//Visibility test
	//if(ion::maths::BoxIntersectsBox(cameraBounds.topLeft, cameraBounds.bottomRight, position, position + m_size))
	{
		ion::Matrix4 transform;

		//Flip
		ion::Vector3 scale(flippedX ? -m_planeSize.x : m_planeSize.x, flippedY ? -m_planeSize.y : m_planeSize.y, 1.0f);
		transform.SetScale(scale);

		//Translate
		transform.SetTranslation(ion::Vector3(position.x, position.y, Constants::Rendering::planePriorities[(int)m_planePriority]));

#if USE_PALETTE_TEXTURES
		Assets::Shaders::IndexTexture::Params::indexedTexture.SetValue(*m_texture);
		Assets::Shaders::IndexTexture::Params::paletteTexture.SetValue(*m_paletteTexture);
#endif

		//Bind material
		m_material->Bind(transform, cameraInv, renderer.GetProjectionMatrix());

		//Draw vertex buffer
		renderer.DrawVertexBuffer(m_primitive->GetVertexBuffer(), m_primitive->GetIndexBuffer());

		//Unbind material
		m_material->Unbind();
	}
}

StampSet::StampSet(const Project& project)
{
	for(TStampMap::const_iterator it = project.StampsBegin(), end = project.StampsEnd(); it != end; ++it)
	{
		//TODO: One for each (used) palette
		m_stamps.insert(std::make_pair(it->first, StampRenderer(it->second, project.GetTileset(), *project.GetPalette(0))));
	}
}