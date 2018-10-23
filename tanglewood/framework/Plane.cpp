///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Plane.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Plane loading/rendering from Beehive map data
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////////

#include "Plane.h"
#include "Shaders.h"

#include <ion/maths/Geometry.h>

Plane::Plane(const Tileset& tileset, const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& size, const Palette& palette)
{
	m_tilesetTexture = nullptr;
	m_material = nullptr;
	m_canvasPrimitive = nullptr;

	//Set size
	m_canvasSize = size;

	//Create render canvases
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;
	m_canvasPrimitive = new ion::render::Chessboard(ion::render::Chessboard::xy, ion::Vector2((float)m_canvasSize.x * (tileWidth / 2.0f), (float)m_canvasSize.y * (tileHeight / 2.0f)), m_canvasSize.x, m_canvasSize.y, true);

	//Create and draw tileset
	CreateTilesetTexture(tileset, palette);

	//Paint map
	PaintMap(tileMap, size);

#if 0
	//Place all stamp instances
	for(TStampPosMap::const_iterator it = stampMap.begin(), end = stampMap.end(); it != end; ++it)
	{
		u32 flags = it->m_flags;

		StampInstance instance;
		instance.position.x = ((float)it->m_position.x * 8);
		instance.position.y = ((float)it->m_position.y * 8);
		instance.flippedX = (flags & Map::eFlipX) != 0;
		instance.flippedY = (flags & Map::eFlipY) != 0;

		std::map<StampId, StampRenderer>::iterator stampRndIt = stampSet.m_stamps.find(it->m_id);
		if (stampRndIt != stampSet.m_stamps.end())
		{
			instance.stamp = &stampRndIt->second;
			m_stampInstances.push_back(instance);
		}
		else
		{
			ion::debug::log << "Plane::Plane() - Could not find stamp id " << it->m_id << ion::debug::end;
		}
	}
#endif
}

Plane::~Plane()
{
	delete m_tilesetTexture;
	delete m_material;
	delete m_canvasPrimitive;
}

void Plane::Render(ion::render::Renderer& renderer, const Bounds& cameraBounds, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize, PlanePriority priority)
{
	ion::Matrix4 transform;

	//Translate
	transform.SetTranslation(ion::Vector3(mapSize.x / 2.0f, mapSize.y / 2.0f, Constants::Rendering::planePriorities[(int)priority]));

#if USE_PALETTE_TEXTURES
	Assets::Shaders::IndexTexture::Params::indexedTexture.SetValue(*m_tilesetTexture);
	Assets::Shaders::IndexTexture::Params::paletteTexture.SetValue(*m_paletteTexture);
#endif

	//Bind material
	m_material->Bind(transform, cameraInv, renderer.GetProjectionMatrix());

	//Draw vertex buffer
	renderer.DrawVertexBuffer(m_canvasPrimitive->GetVertexBuffer(), m_canvasPrimitive->GetIndexBuffer());

	//Unbind material
	m_material->Unbind();

#if 0
	//Visibility test on plane A only
	const bool noVisibilityTest = (priority == PlanePriority::PlaneBLow || priority == PlanePriority::PlaneBHigh);

	for(int i = 0; i < m_stampInstances.size(); i++)
	{
		//TODO: Store in priority lists
		if (m_stampInstances[i].stamp->m_planePriority == priority)
		{
			const ion::Vector2 topLeft = m_stampInstances[i].position;
			const ion::Vector2 bottomRight = topLeft + m_stampInstances[i].stamp->m_size;

			if (noVisibilityTest || ion::maths::BoxIntersectsBox(cameraBounds.topLeft, cameraBounds.bottomRight, topLeft, bottomRight))
			{
				//Centred quad to top-left + draw offset + scroll, inverted for OpenGL
				ion::Vector2 position;
				position.x = m_stampInstances[i].position.x + (m_stampInstances[i].stamp->m_size.x / 2.0f) + m_scroll.x + m_drawOffset.x;
				position.y = mapSize.y - m_stampInstances[i].position.y - (m_stampInstances[i].stamp->m_size.y / 2.0f) + m_scroll.y + m_drawOffset.y;

				m_stampInstances[i].stamp->Render(renderer, position, cameraBounds, cameraInv, m_stampInstances[i].flippedX, m_stampInstances[i].flippedY);
			}
		}
	}
#endif
}

void Plane::CreateTilesetTexture(const Tileset& tileset, const Palette& palette)
{
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;

	u32 numTiles = tileset.GetCount();
	m_tilesetSizeSq = ion::maths::Max(1, (int)ion::maths::Ceil(ion::maths::Sqrt((float)numTiles)));
	u32 textureWidth = m_tilesetSizeSq * tileWidth;
	u32 textureHeight = m_tilesetSizeSq * tileHeight;
	u32 bytesPerPixel = 3;
	u32 textureSize = textureWidth * textureHeight * bytesPerPixel;
	m_cellSizeTexSpaceSq = 1.0f / (float)m_tilesetSizeSq;

	u8* data = new u8[textureSize];
	ion::memory::MemSet(data, 255, textureSize);

	for (int i = 0; i < tileset.GetCount(); i++)
	{
		const Tile& tile = *tileset.GetTile(i);
		PaletteId paletteId = tile.GetPaletteId();

		u32 x = i % m_tilesetSizeSq;
		u32 y = i / m_tilesetSizeSq;

		for (int pixelY = 0; pixelY < tileHeight; pixelY++)
		{
			for (int pixelX = 0; pixelX < tileWidth; pixelX++)
			{
				//Invert Y for OpenGL
				int pixelY_OGL = tileHeight - 1 - pixelY;

				u8 colourIdx = tile.GetPixelColour(pixelX, pixelY_OGL);

				//Protect against blank tiles
				if (palette.IsColourUsed(colourIdx))
				{
					int destPixelX = (x * tileWidth) + pixelX;
					int destPixelY = (y * tileHeight) + pixelY;
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
#endif
				}
			}
		}
	}

	m_tilesetTexture = ion::render::Texture::Create(textureWidth, textureHeight, ion::render::Texture::eRGB, ion::render::Texture::eRGB, ion::render::Texture::eBPP24, false, false, data);
	m_tilesetTexture->SetMinifyFilter(ion::render::Texture::eFilterNearest);
	m_tilesetTexture->SetMagnifyFilter(ion::render::Texture::eFilterNearest);
	m_tilesetTexture->SetWrapping(ion::render::Texture::eWrapClamp);

	delete data;

	//Create material
	m_material = new ion::render::Material();
	m_material->AddDiffuseMap(m_tilesetTexture);
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

void Plane::PaintTile(TileId tileId, int x, int y, u32 tileFlags)
{
	//Set texture coords for cell
	ion::render::TexCoord coords[4];
	ion::render::TexCoord coordsZero[4] = { ion::render::TexCoord(0.0f, 0.0f) };
	GetTileTexCoords(tileId, coords, tileFlags);

	float z = (tileFlags & Map::eHighPlane) ? Constants::Rendering::planePriorities[(int)PlanePriority::PlaneAHigh] : Constants::Rendering::planePriorities[(int)PlanePriority::PlaneALow];

	m_canvasPrimitive->SetTexCoords((y * m_canvasSize.x) + x, coords, z);
}

void Plane::PaintMap(const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& size)
{
	//Paint all tiles
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			//Get tile
			const Map::TileDesc& tileDesc = tileMap[(y * size.x) + x];

			//Get id
			const TileId& tileId = tileDesc.m_id;

			//Invert Y for OpenGL
			int yInv = size.y - 1 - y;

			//Get V/H flip
			u32 tileFlags = tileDesc.m_flags;

			//Paint tile
			PaintTile(tileId, x, yInv, tileFlags);
		}
	}
}

void Plane::GetTileTexCoords(TileId tileId, ion::render::TexCoord texCoords[4], u32 flipFlags) const
{
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;

	if (tileId == InvalidTileId)
	{
		tileId = 0;
	}

	if (tileId == InvalidTileId)
	{
		//Invalid tile, use top-left pixel
		float onePixelTexSpaceX = m_cellSizeTexSpaceSq / tileWidth;
		float onePixelTexSpaceY = m_cellSizeTexSpaceSq / tileHeight;

		//Top left
		texCoords[0].x = 0.0f;
		texCoords[0].y = 0.0f;
		//Bottom left
		texCoords[1].x = 0.0f;
		texCoords[1].y = onePixelTexSpaceY;
		//Bottom right
		texCoords[2].x = onePixelTexSpaceX;
		texCoords[2].y = onePixelTexSpaceY;
		//Top right
		texCoords[3].x = onePixelTexSpaceX;
		texCoords[3].y = 0.0f;
	}
	else
	{
		//Map tile to X/Y on tileset texture
		int tilesetX = (tileId % m_tilesetSizeSq);
		int tilesetY = (tileId / m_tilesetSizeSq);
		ion::Vector2 textureBottomLeft(m_cellSizeTexSpaceSq * tilesetX, m_cellSizeTexSpaceSq * tilesetY);

		bool flipX = (flipFlags & Map::eFlipX) != 0;
		bool flipY = (flipFlags & Map::eFlipY) != 0;

		float top = flipY ? (textureBottomLeft.y) : (textureBottomLeft.y + m_cellSizeTexSpaceSq);
		float left = flipX ? (textureBottomLeft.x + m_cellSizeTexSpaceSq) : (textureBottomLeft.x);
		float bottom = flipY ? (textureBottomLeft.y + m_cellSizeTexSpaceSq) : (textureBottomLeft.y);
		float right = flipX ? (textureBottomLeft.x) : (textureBottomLeft.x + m_cellSizeTexSpaceSq);

		//Top left
		texCoords[0].x = left;
		texCoords[0].y = top;
		//Bottom left
		texCoords[1].x = left;
		texCoords[1].y = bottom;
		//Bottom right
		texCoords[2].x = right;
		texCoords[2].y = bottom;
		//Top right
		texCoords[3].x = right;
		texCoords[3].y = top;
	}
}