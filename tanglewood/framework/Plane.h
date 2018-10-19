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
	Plane(const TStampPosMap& stampMap, StampSet& stampSet);

	void Render(ion::render::Renderer& renderer, const Bounds& cameraBounds, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize, PlanePriority priority);

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

	std::vector<StampInstance> m_stampInstances;
};