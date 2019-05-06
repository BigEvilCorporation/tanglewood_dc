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
#include "Log.h"

#include <ion/maths/Geometry.h>

#include <ion/renderer/imageformats/BMPReader.h>

const std::vector<ion::render::VertexBuffer::Element> PlanePrimitive::s_vertexLayout =
{
	ion::render::VertexBuffer::Element({ ion::render::VertexBuffer::ePosition, ion::render::VertexBuffer::eFloat, 3 }),
	ion::render::VertexBuffer::Element({ ion::render::VertexBuffer::eTexCoord, ion::render::VertexBuffer::eFloat, 2 }),
};

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

	//Create render canvases
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;
	m_canvasPrimitive = new PlanePrimitive(ion::Vector2((float)m_canvasSizeTiles.x * (tileWidth / 2.0f), (float)m_canvasSizeTiles.y * (tileHeight / 2.0f)), m_canvasSizeTiles.x, m_canvasSizeTiles.y);

	//Get vertex buffer
	m_vertexBufferPtrPos = m_canvasPrimitive->GetVertexBuffer().GetStartAddress(ion::render::VertexBuffer::ePosition);
	m_vertexBufferPtrTex = m_canvasPrimitive->GetVertexBuffer().GetStartAddress(ion::render::VertexBuffer::eTexCoord);
	m_vertexStridePos = m_canvasPrimitive->GetVertexBuffer().GetElementSize(ion::render::VertexBuffer::ePosition);
	m_vertexStrideTex = m_canvasPrimitive->GetVertexBuffer().GetElementSize(ion::render::VertexBuffer::eTexCoord);

	u32 copyBufferSize = m_canvasSizeTiles.x * (m_canvasSizeTiles.y - 1) * m_vertexStrideTex * 4;
	m_copyBuffer = new u8[copyBufferSize];

	//Create and draw tileset
	u32 texMemBefore = ion::render::Texture::GetTextureMemoryUsed();
	ion::debug::log << "Loading plane "
		<< " : texture memory used: " << texMemBefore
		<< " (" << (float)texMemBefore / 1024.0f << "kb)" << ion::debug::end;

	CreateTilesetTexture(tileset, palette);

	u32 texMemAfter = ion::render::Texture::GetTextureMemoryUsed();
	ion::debug::log << "Loaded plane "
		<< " (" << (float)texMemAfter / 1024.0f << "kb)"
		<< " cost " << (texMemAfter - texMemBefore) << " (" << (float)(texMemAfter - texMemBefore) / 1024.0f << "kb)" << ion::debug::end;
}

Plane::~Plane()
{
	delete m_tilesetTexture;
	delete m_material;
	delete m_canvasPrimitive;
	delete m_copyBuffer;
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

	m_lastStreamedX = streamColumnX;
	m_lastStreamedY = streamColumnY;
}

void Plane::SetEdgeBehaviour(EdgeBehaviour x, EdgeBehaviour y)
{
	m_edgeBehaviourX = x;
	m_edgeBehaviourY = y;
}

void Plane::SetColourPalette(int paletteIdx)
{
	m_tilesetTexture->SetColourPalette(paletteIdx);
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
		}

		if (streamDirectionX)
		{
			StreamColumn(nextColumn, nextRow, streamDirectionX);
			m_lastStreamedX += streamDirectionX;
		}
	}

	//Wrap camera around scroll buffer size (use stream column/row to match float precision)
	planeCamera.SetTranslation(ion::Vector3(cameraPos.x - (streamColumn * Constants::MegaDrive::tileWidth), cameraPos.y - (streamRow * Constants::MegaDrive::tileHeight), cameraPos.z));

	//Plane draw offset
	ion::Matrix4 transform;
	transform.SetTranslation(ion::Vector3((m_canvasSizeTiles.x * Constants::MegaDrive::tileWidth) / 2.0f, (m_canvasSizeTiles.y * Constants::MegaDrive::tileHeight) / 2.0f, Constants::Rendering::planePriorities[(int)priority]));

#if USE_PALETTE_TEXTURES && defined ION_RENDERER_SHADER
	Assets::Shaders::IndexTexture::Params::indexedTexture.SetValue(*m_tilesetTexture);
	Assets::Shaders::IndexTexture::Params::paletteTexture.SetValue(*m_paletteTexture);
#endif

	//Bind material
	DBG_LOG_LV3("Bind material");
	m_material->Bind(transform, planeCamera.GetInverse(), renderer.GetProjectionMatrix());

	//Draw vertex buffer
	DBG_LOG_LV3("Draw vertex buffer (" << m_canvasPrimitive->GetVertexBuffer().GetData().size() << " bytes), index buffer ( " << m_canvasPrimitive->GetIndexBuffer().GetSize() << " shorts");
	renderer.DrawVertexBuffer(m_canvasPrimitive->GetVertexBuffer(), m_canvasPrimitive->GetIndexBuffer());

	//Unbind material
	DBG_LOG_LV3("Unbind material");
	m_material->Unbind();
}

void Plane::CreateTilesetTexture(const Tileset& tileset, const Palette& palette)
{
	const int tileWidth = Constants::MegaDrive::tileWidth;
	const int tileHeight = Constants::MegaDrive::tileHeight;

	const int tileBorderX = 1;
	const int tileBorderY = 1;

	const int tileWidthBordered = tileWidth + (tileBorderX * 2);
	const int tileHeightBordered = tileHeight + (tileBorderY * 2);

	u32 numTiles = tileset.GetCount();
	m_tilesetSizeSq = ion::maths::Max(1, (int)ion::maths::Ceil(ion::maths::Sqrt((float)numTiles)));
	m_textureSizeSq = ion::maths::NextPowerOfTwo(m_tilesetSizeSq * tileWidthBordered);

#if USE_PALETTES
	u32 bytesPerPixel = 1;
#else
	u32 bytesPerPixel = 4;
#endif

	u32 textureBytes = m_textureSizeSq * m_textureSizeSq * bytesPerPixel;

	m_pixelSizeTexSpace = 1.0f / m_textureSizeSq;
	m_cellSizeTexSpaceSq = m_pixelSizeTexSpace * tileWidthBordered;

	u8* data = new u8[textureBytes];
	ion::memory::MemSet(data, 255, textureBytes);

	for (int i = 0; i < tileset.GetCount(); i++)
	{
		const Tile& tile = *tileset.GetTile(i);
		PaletteId paletteId = tile.GetPaletteId();

		u32 x = i % m_tilesetSizeSq;
		u32 y = i / m_tilesetSizeSq;

		for (int pixelY = 0; pixelY < tileHeightBordered; pixelY++)
		{
			for (int pixelX = 0; pixelX < tileWidthBordered; pixelX++)
			{
				int pixelXBordered = ion::maths::Clamp(pixelX - tileBorderX, 0, tileWidth - 1);
				int pixelYBordered = ion::maths::Clamp(pixelY - tileBorderY, 0, tileHeight - 1);

				//Invert Y for OpenGL
				int pixelY_OGL = tileHeight - 1 - pixelYBordered;

				u8 colourIdx = tile.GetPixelColour(pixelXBordered, pixelY_OGL);

				//Protect against blank tiles
				if (palette.IsColourUsed(colourIdx))
				{
					int destPixelX = (x * tileWidthBordered) + pixelX;
					int destPixelY = (y * tileHeightBordered) + pixelY;
					u32 pixelIdx = (destPixelY * m_textureSizeSq) + destPixelX;
					u32 dataOffset = pixelIdx * bytesPerPixel;
					ion::debug::Assert(dataOffset + bytesPerPixel <= textureBytes, "eOut of bounds");

#if USE_PALETTES
					data[dataOffset] = colourIdx;
#else
					const Colour& colour = palette.GetColour(colourIdx);
					data[dataOffset] = colour.GetRed();
					data[dataOffset + 1] = colour.GetGreen();
					data[dataOffset + 2] = colour.GetBlue();
					data[dataOffset + 3] = (colourIdx > 0) ? 255 : 0;	//Colour 0 is transparent
#endif
				}
			}
		}
	}

#if USE_PALETTES
#if defined ION_RENDERER_SHADER
	//Palettes using shaders
	ion::render::Texture::Format format = ion::render::Texture::Format::R;
	ion::render::Texture::BitsPerPixel bpp = ion::render::Texture::BitsPerPixel::BPP8;
#else
	//Palettes using fixed function
	ion::render::Texture::Format format = ion::render::Texture::Format::RGBA_Indexed;
	ion::render::Texture::BitsPerPixel bpp = ion::render::Texture::BitsPerPixel::BPP8;
#endif
#else
	ion::render::Texture::Format format = ion::render::Texture::Format::RGBA;
	ion::render::Texture::BitsPerPixel bpp = ion::render::Texture::BitsPerPixel::BPP24;
#endif

	m_tilesetTexture = ion::render::Texture::Create(m_textureSizeSq, m_textureSizeSq, format, format, bpp, false, false, data);

	m_tilesetTexture->SetMinifyFilter(ion::render::Texture::Filter::Nearest);
	m_tilesetTexture->SetMagnifyFilter(ion::render::Texture::Filter::Nearest);
	m_tilesetTexture->SetWrapping(ion::render::Texture::Wrapping::Clamp);

	//Set default palette index
	m_tilesetTexture->SetColourPalette(Constants::Palettes::palIndexWorld);

	delete [] data;

	//Create material
	m_material = new ion::render::Material();
	m_material->AddDiffuseMap(m_tilesetTexture);
	m_material->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f));

#if defined ION_RENDERER_SHADER
#if USE_PALETTES
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
	GetTileTexCoords(tileId, coords, tileFlags);

	const float z = (tileFlags & Map::eHighPlane) ? Constants::Rendering::planePriorities[(int)PlanePriority::PlaneAHigh] : Constants::Rendering::planePriorities[(int)PlanePriority::PlaneALow];

	//4 verts per tile
	u8* ptrPos = m_vertexBufferPtrPos + (m_vertexStridePos * index * 4);
	u8* ptrTex = m_vertexBufferPtrTex + (m_vertexStrideTex * index * 4);

	for (int i = 0; i < 4; i++)
	{
		ion::Vector3* position = (ion::Vector3*)ptrPos;
		ion::render::TexCoord* texcoord = (ion::render::TexCoord*)ptrTex;

		*texcoord = coords[i];
		position->z = z;

		ptrPos += m_vertexStridePos;
		ptrTex += m_vertexStrideTex;
	}
}

void Plane::GetTileTexCoords(TileId tileId, ion::render::TexCoord texCoords[4], u32 flipFlags) const
{
	if (tileId == InvalidTileId)
	{
		tileId = 0;
	}

	if (tileId == InvalidTileId)
	{
		//Invalid tile, use top-left pixel

		//Top left
		texCoords[0].x = 0.0f;
		texCoords[0].y = 0.0f;
		//Bottom left
		texCoords[1].x = 0.0f;
		texCoords[1].y = m_pixelSizeTexSpace;
		//Bottom right
		texCoords[2].x = m_pixelSizeTexSpace;
		texCoords[2].y = m_pixelSizeTexSpace;
		//Top right
		texCoords[3].x = m_pixelSizeTexSpace;
		texCoords[3].y = 0.0f;
	}
	else
	{
		//Map tile to X/Y on tileset texture
		int tilesetX = (tileId % m_tilesetSizeSq);
		int tilesetY = (tileId / m_tilesetSizeSq);
		ion::Vector2 textureBottomLeft((m_cellSizeTexSpaceSq * tilesetX) + m_pixelSizeTexSpace, (m_cellSizeTexSpaceSq * tilesetY) + m_pixelSizeTexSpace);
		ion::Vector2 textureTopRight((m_cellSizeTexSpaceSq * (tilesetX + 1)) - m_pixelSizeTexSpace, (m_cellSizeTexSpaceSq * (tilesetY + 1)) - m_pixelSizeTexSpace);

		bool flipX = (flipFlags & Map::eFlipX) != 0;
		bool flipY = (flipFlags & Map::eFlipY) != 0;

		float top = flipY ? (textureBottomLeft.y) : textureTopRight.y;
		float left = flipX ? textureTopRight.x : (textureBottomLeft.x);
		float bottom = flipY ? textureTopRight.y : (textureBottomLeft.y);
		float right = flipX ? (textureBottomLeft.x) : textureTopRight.x;

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
	//Shift one row at a time
	u32 copySize = ((m_canvasSizeTiles.x - 1) * m_vertexStrideTex * 4);

	for (int y = 0; y < m_canvasSizeTiles.y; y++)
	{
		u8* srcPtr = m_vertexBufferPtrTex + (m_canvasSizeTiles.x * y * m_vertexStrideTex * 4);
		u8* dstPtr = srcPtr + (m_vertexStrideTex * 4);

		if (direction < 0)
		{
			std::swap(srcPtr, dstPtr);
		}

		ion::memory::MemCopy(dstPtr, srcPtr, copySize);
	}
}

void Plane::ShiftMapY(int direction)
{
	//4 verts per cell
	u32 copySize = m_canvasSizeTiles.x * (m_canvasSizeTiles.y - 1) * m_vertexStrideTex * 4;
	u8* srcPtr = m_vertexBufferPtrTex;
	u8* dstPtr = m_vertexBufferPtrTex + (m_canvasSizeTiles.x * m_vertexStrideTex * 4);

	if (direction < 0)
	{
		std::swap(srcPtr, dstPtr);
	}

	ion::memory::MemCopy(m_copyBuffer, srcPtr, copySize);
	ion::memory::MemCopy(dstPtr, m_copyBuffer, copySize);
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

PlanePrimitive::PlanePrimitive(const ion::Vector2& halfExtents, int widthCells, int heightCells)
	: ion::render::Primitive(ion::render::VertexBuffer::eTriangles, s_vertexLayout, ion::render::VertexBuffer::eContiguous)
{
	ion::Vector2 cellSize((halfExtents.x * 2.0f) / (float)widthCells, (halfExtents.y * 2.0f) / (float)heightCells);

	int vertexCount = 0;
	for (int y = 0; y < heightCells; y++)
	{
		for (int x = 0; x < widthCells; x++)
		{
				//Create quad per cell
				ion::Vector2 cellPos((cellSize.x * x) - halfExtents.x, (cellSize.y * y) - halfExtents.y);
				m_vertexBuffer.AddVertex(ion::Vector3(cellPos.x, cellPos.y + cellSize.y, 0.0f),				ion::Vector3(0.0f, 0.0f, 1.0f), ion::Colour(1.0f, 1.0f, 1.0f, 1.0f), ion::render::TexCoord(0.0f, 1.0f));
				m_vertexBuffer.AddVertex(ion::Vector3(cellPos.x, cellPos.y, 0.0f),							ion::Vector3(0.0f, 0.0f, 1.0f), ion::Colour(1.0f, 1.0f, 1.0f, 1.0f), ion::render::TexCoord(1.0f, 1.0f));
				m_vertexBuffer.AddVertex(ion::Vector3(cellPos.x + cellSize.x, cellPos.y, 0.0f),				ion::Vector3(0.0f, 0.0f, 1.0f), ion::Colour(1.0f, 1.0f, 1.0f, 1.0f), ion::render::TexCoord(1.0f, 0.0f));
				m_vertexBuffer.AddVertex(ion::Vector3(cellPos.x + cellSize.x, cellPos.y + cellSize.y, 0.0f),ion::Vector3(0.0f, 0.0f, 1.0f), ion::Colour(1.0f, 1.0f, 1.0f, 1.0f), ion::render::TexCoord(0.0f, 0.0f));

				m_indexBuffer.Add(vertexCount, vertexCount + 1, vertexCount + 2);
				m_indexBuffer.Add(vertexCount, vertexCount + 2, vertexCount + 3);

				vertexCount += 4;
		}
	}
}