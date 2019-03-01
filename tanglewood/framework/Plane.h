///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Plane.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Plane loading/rendering from Beehive map data
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Renderer.h>
#include <ion/renderer/Camera.h>
#include <ion/renderer/TexCoord.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Material.h>
#include <ion/beehive/Map.h>
#include <ion/beehive/Stamp.h>

#include <map>
#include <vector>

#include "Constants.h"
#include "PlanePriority.h"

class PlanePrimitive : public ion::render::Primitive
{
public:
	PlanePrimitive(const ion::Vector2& halfExtents, int widthCells, int heightCells);

private:
	static const std::vector<ion::render::VertexBuffer::Element> s_vertexLayout;
};

class Plane
{
public:
	enum class EdgeBehaviour
	{
		Clamp,
		Wrap
	};

	Plane(const Tileset& tileset, const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& mapSizeTiles, const ion::Vector2i& canvasSizeTiles, const Palette& palette);
	~Plane();

	void SetEdgeBehaviour(EdgeBehaviour x, EdgeBehaviour y);
	void PreStream(const ion::render::Camera& camera);

	void Render(ion::render::Renderer& renderer, const ion::render::Camera* camera, PlanePriority priority);

#if USE_PALETTE_TEXTURES
	void SetPaletteTexture(ion::render::Texture* texture) { m_paletteTexture = texture; }
#endif

	void SetColourPalette(int paletteIdx);

	ion::Vector2 m_scroll;
	ion::Vector2 m_drawOffset;

private:
	struct CachedTile
	{
		TileId id;
		u32 flags;
	};

	void CreateTilesetTexture(const Tileset& tileset, const Palette& palette);
	void PaintTile(TileId tileId, int x, int y, u32 flipFlags);
	void GetTileTexCoords(TileId tileId, ion::render::TexCoord texCoords[4], u32 tileFlags) const;

	void ShiftMapX(int direction);
	void ShiftMapY(int direction);
	void StreamColumn(int x, int y, int direction);
	void StreamRow(int x, int y, int direction);

	EdgeBehaviour m_edgeBehaviourX;
	EdgeBehaviour m_edgeBehaviourY;

	const std::vector<Map::TileDesc>& m_tileMap;
	std::vector<CachedTile> m_tileCache;

	ion::render::Texture* m_tilesetTexture;
	PlanePrimitive* m_canvasPrimitive;
	ion::render::Material* m_material;

#if USE_PALETTE_TEXTURES
	ion::render::Texture* m_paletteTexture;
#endif

	u8* m_vertexBufferPtr;
	u32 m_vertexStride;
	u32 m_vertexOffsetPosition;
	u32 m_vertexOffsetTexCoord;

	ion::Vector2i m_mapSizeTiles;
	ion::Vector2i m_canvasSizeTiles;
	u32 m_tilesetSizeSq;
	u32 m_textureSizeSq;
	float m_pixelSizeTexSpace;
	float m_cellSizeTexSpaceSq;

	int m_lastStreamedX;
	int m_lastStreamedY;
};