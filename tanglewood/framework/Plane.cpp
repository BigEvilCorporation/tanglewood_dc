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

Plane::Plane(const Map& map, StampSet& stampSet)
{
	//Place all stamp instances
	for(TStampPosMap::const_iterator it = map.StampsBegin(), end = map.StampsEnd(); it != end; ++it)
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
}

void Plane::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize, PlanePriority priority)
{
	for(int i = 0; i < m_stampInstances.size(); i++)
	{
		if (m_stampInstances[i].stamp->m_planePriority == priority)
		{
			//Centred quad to top-left + draw offset + scroll, inverted for OpenGL
			ion::Vector2 position;
			position.x = m_stampInstances[i].position.x + (m_stampInstances[i].stamp->m_size.x / 2.0f) + m_scroll.x + m_drawOffset.x;
			position.y = mapSize.y - m_stampInstances[i].position.y - (m_stampInstances[i].stamp->m_size.y / 2.0f) + m_scroll.y + m_drawOffset.y;

			m_stampInstances[i].stamp->Render(renderer, position, cameraInv, m_stampInstances[i].flippedX, m_stampInstances[i].flippedY);
		}
	}
}