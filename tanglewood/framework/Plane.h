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
#include <ion/beehive/Map.h>
#include <ion/beehive/Stamp.h>

#include <map>
#include <vector>

#include "Stamp.h"

class Plane
{
public:
	Plane(const Tileset& tileset, const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& size, const Palette& palette);
	~Plane();

	void Render(ion::render::Renderer& renderer, const Bounds& cameraBounds, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize, PlanePriority priority);

#if USE_PALETTE_TEXTURES
	void SetPaletteTexture(ion::render::Texture* texture) { m_paletteTexture = texture; }
#endif

	ion::Vector2 m_scroll;
	ion::Vector2 m_drawOffset;

private:
	struct StampInstance
	{
		ion::Vector2 position;
		bool flippedX;
		bool flippedY;
		StampRenderer* stamp;
	};

	void CreateTilesetTexture(const Tileset& tileset, const Palette& palette);
	void PaintTile(TileId tileId, int x, int y, u32 flipFlags);
	void PaintMap(const std::vector<Map::TileDesc>& tileMap, const ion::Vector2i& size);
	void GetTileTexCoords(TileId tileId, ion::render::TexCoord texCoords[4], u32 tileFlags) const;

	std::vector<StampInstance> m_stampInstances;

	ion::render::Texture* m_tilesetTexture;
	ion::render::Chessboard* m_canvasPrimitive;
	ion::render::Material* m_material;

#if USE_PALETTE_TEXTURES
	ion::render::Texture* m_paletteTexture;
#endif

	ion::Vector2i m_canvasSize;
	u32 m_tilesetSizeSq;
	float m_cellSizeTexSpaceSq;
};