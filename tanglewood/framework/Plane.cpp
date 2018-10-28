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
#include "Globals.h"

#include <ion/maths/Geometry.h>

Plane::Plane(const Tileset& tileset, const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& mapSizeTiles, const ion::Vector2i& canvasSizeTiles, const Palette& palette)
	: m_tileMap(tileMap)
{
	m_tilesetTexture = nullptr;
	m_material = nullptr;
	m_canvasPrimitive = nullptr;

	m_edgeBehaviourX = EdgeBehaviour::Wrap;
	m_edgeBehaviourY = EdgeBehaviour::Wrap;

	//Set size (+2 tile border for scroll buffer)
	m_canvasSizeTiles = canvasSizeTiles + ion::Vector2i(4, 4);
	m_mapSizeTiles = mapSizeTiles;

	//Alloc render tile buffer
	m_renderTiles.resize(m_canvasSizeTiles.x * m_canvasSizeTiles.y);

	//Create render canvases
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;
	m_canvasPrimitive = new ion::render::Chessboard(ion::render::Chessboard::xy, ion::Vector2((float)m_canvasSizeTiles.x * (tileWidth / 2.0f), (float)m_canvasSizeTiles.y * (tileHeight / 2.0f)), m_canvasSizeTiles.x, m_canvasSizeTiles.y, true);

	//Create and draw tileset
	CreateTilesetTexture(tileset, palette);
}

Plane::~Plane()
{
	delete m_tilesetTexture;
	delete m_material;
	delete m_canvasPrimitive;
}

void Plane::PreStream(const ion::render::Camera& camera)
{
	ion::Vector3 cameraPos = camera.GetTransform().GetTranslation();
	int streamColumnX = cameraPos.x / 8.0f;
	int streamColumnY = m_mapSizeTiles.y - 1 - (cameraPos.y / 8.0f) - m_canvasSizeTiles.y;

	//Stream all columns
	for (int x = 0; x < m_canvasSizeTiles.x; x++)
	{
		int srcX = streamColumnX + x;
		int srcY = streamColumnY;

		int dstX = x;

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

	//Copy render tiles to canvas
	for (int i = 0; i < m_renderTiles.size(); i++)
	{
		m_canvasPrimitive->SetTexCoords(i, m_renderTiles[i].coords, m_renderTiles[i].z);
	}

	m_lastStreamedX = streamColumnX;
	m_lastStreamedY = streamColumnY;
}

void Plane::SetEdgeBehaviour(EdgeBehaviour x, EdgeBehaviour y)
{
	m_edgeBehaviourX = x;
	m_edgeBehaviourY = y;
}

void Plane::Render(ion::render::Renderer& renderer, const ion::render::Camera* camera, PlanePriority priority)
{
	//Get camera pos
	ion::Matrix4 planeCamera;
	ion::Vector3 cameraPos;
	
	if (camera)
	{
		planeCamera = camera->GetTransform();
		cameraPos = camera->GetTransform().GetTranslation();
	}
	else
	{
		ion::Vector3 cameraZoom;
		cameraZoom.x = (float)Globals::Rendering::windowWidth / (float)Globals::Rendering::gameCanvasWidth;
		cameraZoom.y = (float)Globals::Rendering::windowHeight / (float)Globals::Rendering::gameCanvasHeight;
		cameraZoom.z = 1.0f;

		planeCamera.SetScale(ion::Vector3(1.0f, 1.0f, 1.0f) / cameraZoom);

		cameraPos = ion::Vector3(m_scroll.x, m_scroll.y, -0.1f);
	}

	//Determine next column/row to stream
	int streamColumn = ion::maths::Floor(cameraPos.x / Constants::MegaDrive::tileWidth);
	int streamRow = ion::maths::Floor(cameraPos.y / Constants::MegaDrive::tileHeight);
	int streamRowInv = m_mapSizeTiles.y - streamRow - m_canvasSizeTiles.y;

	bool redraw = false;

	while (streamColumn != m_lastStreamedX || streamRowInv != m_lastStreamedY)
	{
		int streamDirectionX = 0;
		int streamDirectionY = 0;

		//Shift map first
		if (streamRowInv > m_lastStreamedY)
		{
			ShiftMapY(1);
			streamDirectionY = 1;
		}
		else if (streamRowInv < m_lastStreamedY)
		{
			ShiftMapY(-1);
			streamDirectionY = -1;
		}

		if (streamColumn > m_lastStreamedX)
		{
			ShiftMapX(-1);
			streamDirectionX = 1;
		}
		else if (streamColumn < m_lastStreamedX)
		{
			ShiftMapX(1);
			streamDirectionX = -1;
		}

		int nextColumn = m_lastStreamedX + streamDirectionX;
		int nextRow = m_lastStreamedY + streamDirectionY;

		//Stream new columns/rows
		if (streamDirectionY)
		{
			StreamRow(nextColumn, nextRow, streamDirectionY);
			m_lastStreamedY += streamDirectionY;
			redraw = true;
		}

		if (streamDirectionX)
		{
			StreamColumn(nextColumn, nextRow, streamDirectionX);
			m_lastStreamedX += streamDirectionX;
			redraw = true;
		}
	}

	if (redraw)
	{
		//Blit to canvas
		for (int i = 0; i < m_renderTiles.size(); i++)
		{
			m_canvasPrimitive->SetTexCoords(i, m_renderTiles[i].coords, m_renderTiles[i].z);
		}
	}

	//Wrap camera around scroll buffer size (use stream column/row to match float precision)
	planeCamera.SetTranslation(ion::Vector3(cameraPos.x - (streamColumn * Constants::MegaDrive::tileWidth), cameraPos.y - (streamRow * Constants::MegaDrive::tileHeight), cameraPos.z));

	//Plane draw offset
	ion::Matrix4 transform;
	transform.SetTranslation(ion::Vector3((m_canvasSizeTiles.x * Constants::MegaDrive::tileWidth) / 2.0f, (m_canvasSizeTiles.y * Constants::MegaDrive::tileHeight) / 2.0f, Constants::Rendering::planePriorities[(int)priority]));

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

	//Stream in next column
	for (int dstY = 0; dstY < m_canvasSizeTiles.y; dstY++)
	{
		//Get tile
		int tileIndex = 0;

		if (m_edgeBehaviourX == EdgeBehaviour::Clamp)
		{
			if (srcX >= 0 && srcX < m_mapSizeTiles.x && srcY >= 0 && srcY < m_mapSizeTiles.y)
			{
				tileIndex = (srcY * m_mapSizeTiles.x) + srcX;
			}
		}
		else
		{
			tileIndex = (ion::maths::Wrap(srcY, m_mapSizeTiles.y) * m_mapSizeTiles.x) + ion::maths::Wrap(srcX, m_mapSizeTiles.x);
		}

		const Map::TileDesc& tileDesc = m_tileMap[tileIndex];

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

	//Stream in next row
	for (int dstX = 0; dstX < m_canvasSizeTiles.x; dstX++)
	{
		//Get tile
		int tileIndex = 0;

		if (m_edgeBehaviourX == EdgeBehaviour::Clamp)
		{
			if (srcX >= 0 && srcX < m_mapSizeTiles.x && srcY >= 0 && srcY < m_mapSizeTiles.y)
			{
				tileIndex = (srcY * m_mapSizeTiles.x) + srcX;
			}
		}
		else
		{
			tileIndex = (ion::maths::Wrap(srcY, m_mapSizeTiles.y) * m_mapSizeTiles.x) + ion::maths::Wrap(srcX, m_mapSizeTiles.x);
		}

		const Map::TileDesc& tileDesc = m_tileMap[tileIndex];

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
