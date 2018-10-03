///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Stamp.h
// Date:		13th January 2017
// Authors:		Matt Phillips
// Description:	Stamp loading/rendering from Beehive data
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Primitive.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/renderer/Renderer.h>
#include <ion/beehive/Project.h>
#include <ion/beehive/Stamp.h>

#include "PlanePriority.h"

#include <map>

class StampRenderer
{
public:
	StampRenderer() {}

	//Load stamp from Beehive data
	StampRenderer(const Stamp& stamp, const Tileset& tileset, const Palette& palette);

	//Render
	void Render(ion::render::Renderer& renderer, const ion::Vector2& position, const ion::Matrix4& cameraInv, bool flippedX, bool flippedY);

	//Size (pixels)
	ion::Vector2 m_size;

	//Plane
	PlanePriority m_planePriority;

private:
	ion::render::Texture* m_texture;
	ion::render::Material* m_material;
	ion::render::Quad* m_primitive;
};

class StampSet
{
public:
	//Load from Beehive data
	StampSet(const Project& project);

	std::map<StampId, StampRenderer> m_stamps;
};
