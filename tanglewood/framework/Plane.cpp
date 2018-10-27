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

Plane::Plane(const Tileset& tileset, const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& mapSizeTiles, const ion::Vector2i& canvasSizeTiles, const Palette& palette)
	: m_tileMap(tileMap)
{
	m_tilesetTexture = nullptr;
	m_material = nullptr;
	m_canvasPrimitive = nullptr;

	//Set size (+1 tile border for scroll buffer)
	m_canvasSizeTiles = canvasSizeTiles + ion::Vector2i(2, 2);
	m_mapSizeTiles = mapSizeTiles;

	//Alloc render tile buffer
	m_renderTiles.resize(m_canvasSizeTiles.x * m_canvasSizeTiles.y);

	//Create render canvases
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;
	m_canvasPrimitive = new ion::render::Chessboard(ion::render::Chessboard::xy, ion::Vector2((float)m_canvasSizeTiles.x * (tileWidth / 2.0f), (float)m_canvasSizeTiles.y * (tileHeight / 2.0f)), m_canvasSizeTiles.x, m_canvasSizeTiles.y, true);

	//Create and draw tileset
	CreateTilesetTexture(tileset, palette);

	//Paint map
	PaintMap();

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

void Plane::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, PlanePriority priority)
{
	ion::Matrix4 transform;

	//Translate
	transform.SetTranslation(ion::Vector3((m_canvasSizeTiles.x * 8.0f) / 2.0f, (m_canvasSizeTiles.y * 8.0f) / 2.0f, Constants::Rendering::planePriorities[(int)priority]));

	//Wrap camera around scroll buffer size
	ion::Matrix4 planeCamera = camera.GetTransform();
	ion::Vector3 cameraZoom = camera.GetZoom();
	ion::Vector3 cameraTransform = planeCamera.GetTranslation();
	planeCamera.SetTranslation(ion::Vector3(ion::maths::Fmod(cameraTransform.x, 8.0f), ion::maths::Fmod(cameraTransform.y, 8.0f), cameraTransform.z));

	bool redraw = false;

	int streamColumnX = cameraTransform.x / 8.0f;
	int streamColumnY = m_mapSizeTiles.y - 1 - (cameraTransform.y / 8.0f) - m_canvasSizeTiles.y;

	if (streamColumnX > m_lastStreamedX)
	{
		ShiftMapX(-1);
		StreamColumn(streamColumnX, streamColumnY, 1);
		redraw = true;
	}
	else if (streamColumnX < m_lastStreamedX)
	{
		ShiftMapX(1);
		StreamColumn(streamColumnX, streamColumnY, -1);
		redraw = true;
	}

	if (streamColumnY > m_lastStreamedY)
	{
		ShiftMapY(1);
		StreamRow(streamColumnX, streamColumnY, 1);
		redraw = true;
	}
	else if (streamColumnY < m_lastStreamedY)
	{
		ShiftMapY(-1);
		StreamRow(streamColumnX, streamColumnY, -1);
		redraw = true;
	}

	m_lastStreamedX = streamColumnX;
	m_lastStreamedY = streamColumnY;

	//Copy render tiles to canvas
	for (int i = 0; i < m_renderTiles.size(); i++)
	{
		m_canvasPrimitive->SetTexCoords(i, m_renderTiles[i].coords, m_renderTiles[i].z);
	}

#if USE_PALETTE_TEXTURES
	Assets::Shaders::IndexTexture::Params::indexedTexture.SetValue(*m_tilesetTexture);
	Assets::Shaders::IndexTexture::Params::paletteTexture.SetValue(*m_paletteTexture);
#endif

	//Bind material
	m_material->Bind(transform, planeCamera.GetInverse(), renderer.GetProjectionMatrix());

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
	//Get tile index
	int index = (y * m_canvasSizeTiles.x) + x;

	//Set texture coords for cell
	ion::render::TexCoord coords[4];
	ion::render::TexCoord coordsZero[4] = { ion::render::TexCoord(0.0f, 0.0f) };
	GetTileTexCoords(tileId, m_renderTiles[index].coords, tileFlags);

	//Set z
	m_renderTiles[index].z = (tileFlags & Map::eHighPlane) ? Constants::Rendering::planePriorities[(int)PlanePriority::PlaneAHigh] : Constants::Rendering::planePriorities[(int)PlanePriority::PlaneALow];
}

void Plane::PaintMap()
{
	//Paint all tiles
	for (int y = 0; y < m_canvasSizeTiles.y; y++)
	{
		for (int x = 0; x < m_canvasSizeTiles.x; x++)
		{
			//Get tile
			const Map::TileDesc& tileDesc = m_tileMap[(y * m_mapSizeTiles.x) + x];

			//Get id
			const TileId& tileId = tileDesc.m_id;

			//Invert Y for OpenGL
			int yInv = m_canvasSizeTiles.y - 1 - y;

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

void Plane::ShiftMapX(int direction)
{
	//Shifting from left to right
	int startX = m_canvasSizeTiles.x - 1;
	int endX = 0;
	int incrementX = -1;

	if (direction < 0)
	{
		//Shifting from right to left
		startX = 0;
		endX = m_canvasSizeTiles.x - 1;
		incrementX = 1;
	}

	//Shift one column at a time
	for (int x = startX; x != endX; x += incrementX)
	{
		for (int y = 0; y < m_canvasSizeTiles.y; y++)
		{
			int indexSrc = (y * m_canvasSizeTiles.x) + x + incrementX;
			int indexDst = (y * m_canvasSizeTiles.x) + x;

			m_renderTiles[indexDst] = m_renderTiles[indexSrc];
		}
	}
}

void Plane::ShiftMapY(int direction)
{
	//Shifting from top to bottom
	int startY = m_canvasSizeTiles.y - 1;
	int endY = 0;
	int incrementY = -1;

	if (direction < 0)
	{
		//Shifting from bottom to top
		startY = 0;
		endY = m_canvasSizeTiles.y - 1;
		incrementY = 1;
	}

	//Shift one column at a time
	for (int y = startY; y != endY; y += incrementY)
	{
		for (int x = 0; x < m_canvasSizeTiles.x; x++)
		{
			int indexSrc = ((y + incrementY) * m_canvasSizeTiles.x) + x;
			int indexDst = (y * m_canvasSizeTiles.x) + x;

			m_renderTiles[indexDst] = m_renderTiles[indexSrc];
		}
	}
}

void Plane::StreamColumn(int x, int y, int direction)
{	
	int srcX = x + (direction > 0 ? (m_canvasSizeTiles.x - 1) : 0);
	int srcY = y;

	int dstX = direction > 0 ? (m_canvasSizeTiles.x - 1) : 0;

	//Clamp source X to map bounds
	srcX = ion::maths::Clamp(srcX, 0, m_mapSizeTiles.x - 1);

	//Stream in next column
	for (int dstY = 0; dstY < m_canvasSizeTiles.y; dstY++)
	{
		//Clamp source Y to map bounds
		srcY = ion::maths::Clamp(srcY, 0, m_mapSizeTiles.y - 1);

		//Get tile
		const Map::TileDesc& tileDesc = m_tileMap[(srcY * m_mapSizeTiles.x) + srcX];

		//Get id
		const TileId& tileId = tileDesc.m_id;

		//Get V/H flip
		u32 tileFlags = tileDesc.m_flags;

		//Invert dest Y for OpenGL
		int dstYinv = m_canvasSizeTiles.y - 1 - dstY;

		//Paint tile
		PaintTile(tileId, dstX, dstYinv, tileFlags);

		//Next source Y
		srcY++;
	}
}

void Plane::StreamRow(int x, int y, int direction)
{
	int srcX = x;
	int srcY = y + (direction > 0 ? (m_canvasSizeTiles.y - 1) : 0);

	int dstY = direction > 0 ? (m_canvasSizeTiles.y - 1) : 0;

	//Clamp source Y to map bounds
	srcY = ion::maths::Clamp(srcY, 0, m_mapSizeTiles.y - 1);

	//Stream in next row
	for (int dstX = 0; dstX < m_canvasSizeTiles.x; dstX++)
	{
		//Clamp source X to map bounds
		srcX = ion::maths::Clamp(srcX, 0, m_mapSizeTiles.x - 1);

		//Get tile
		const Map::TileDesc& tileDesc = m_tileMap[(srcY * m_mapSizeTiles.x) + srcX];

		//Get id
		const TileId& tileId = tileDesc.m_id;

		//Get V/H flip
		u32 tileFlags = tileDesc.m_flags;

		//Invert dest Y for OpenGL
		int dstYinv = m_canvasSizeTiles.y - 1 - dstY;

		//Paint tile
		PaintTile(tileId, dstX, dstYinv, tileFlags);

		//Next source X
		srcX++;
	}
}
