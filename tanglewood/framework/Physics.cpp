///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsWorld.cpp
// Date:		30th September 2018
// Authors:		Matt Phillips
// Description:	Physics world
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Physics.h"
#include "Constants.h"
#include "PhysicsObj.h"

#include <ion/beehive/Project.h>
#include <ion/beehive/Map.h>
#include <ion/core/utils/STL.h>

PhysicsWorld::PhysicsWorld()
{
	m_gravity = Constants::World::defaultGravity;
	m_speedScale = 1.0f;
}

void PhysicsWorld::AddObject(PhysicsObj& physicsObj)
{
	m_physicsObjs.push_back(&physicsObj);
}

void PhysicsWorld::RemoveObject(PhysicsObj& physicsObj)
{
	ion::utils::stl::FindAndRemove(m_physicsObjs, &physicsObj);
}

void PhysicsWorld::AddPlatform(Platform& platform)
{
	m_platforms.push_back(&platform);
}

void PhysicsWorld::RemovePlatform(Platform& platform)
{
	ion::utils::stl::FindAndRemove(m_platforms, &platform);
}

void PhysicsWorld::AddBarrier(Barrier& barrier)
{
	m_barriers.push_back(&barrier);
}

void PhysicsWorld::RemoveBarrier(Barrier& barrier)
{
	ion::utils::stl::FindAndRemove(m_barriers, &barrier);
}

void PhysicsWorld::LoadWorld(const std::string& terrainTilesetFilename, const std::string& collisionMapFilename)
{
	ion::io::File tilesetFile(terrainTilesetFilename, ion::io::File::eOpenRead);
	if (tilesetFile.IsOpen())
	{
		ion::io::Archive archive(tilesetFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		m_terrainTileset.Serialise(archive);
		tilesetFile.Close();
	}

	ion::io::File mapFile(collisionMapFilename, ion::io::File::eOpenRead);
	if (mapFile.IsOpen())
	{
		ion::io::Archive archive(mapFile, ion::io::Archive::Direction::In);
		archive.SetContentType(ion::io::Archive::Content::Minimal);
		archive.Serialise(m_collisionMap, "collisionMap");
		mapFile.Close();
	}
}

void PhysicsWorld::RemoveAllObjects()
{
	m_physicsObjs.clear();
	m_pushableObjs.clear();
	m_platforms.clear();
	m_barriers.clear();
}

void PhysicsWorld::Step(float deltaTime)
{
	for (int i = 0; i < m_physicsObjs.size(); i++)
	{
		m_physicsObjs[i]->PhysicsStep(deltaTime, *this);
	}
}

int PhysicsWorld::FindFloor(const ion::Vector2i& position, int maxSearchLength, u16& tileFlags) const
{
	//Position to starting tile
	ion::Vector2i tilePos(position.x / 8, position.y / 8);

	int tileHeight = 0;
	bool found = false;

	if (tilePos.x >= 0 && tilePos.x < m_collisionMap.GetWidth())
	{
		//X offset
		int offsetX = position.x % 8;

		int lengthSearched = 0;
		int solidTilesFound = 0;
		int hollowTilesFound = 0;

		while (!found && lengthSearched <= maxSearchLength && tilePos.y >= 0 && tilePos.y < m_collisionMap.GetHeight())
		{
			//Assume hollow if no tile
			int height = 0;

			//Get terrain tile id
			TerrainTileId terrainTileId = m_collisionMap.GetTerrainTile(tilePos.x, tilePos.y);

			//Get terrain tile
			if (const TerrainTile* terrainTile = m_terrainTileset.GetTerrainTile(terrainTileId))
			{
				//Get height at offset
				height = (int)terrainTile->GetHeight(offsetX);
			}

			if (height == 8)
			{
				//Solid tile
				solidTilesFound++;

				if (hollowTilesFound > 0)
				{
					//Last tile was hollow, found terrain
					found = true;
				}
				else
				{
					//Search upwards
					tilePos.y--;
				}
			}
			else if (height == 0)
			{
				//Hollow tile
				hollowTilesFound++;

				if (solidTilesFound > 0)
				{
					//Last tile was solid, found terrain
					found = true;
				}
				else
				{
					//Search downwards
					tilePos.y++;
				}

				lengthSearched += 8;
			}
			else
			{
				//Found
				found = true;
			}

			tileHeight = height;
		}
	}

	if (found)
	{
		//Get flags
		tileFlags = m_collisionMap.GetCollisionTileFlags(tilePos.x, tilePos.y);

		//Tile to pixel space + total height accumulated - 1 tile
		return ((tilePos.y + 1) * 8) - tileHeight;
	}
	else
	{
		//No terrain within search distance
		tileFlags = 0;
		return -1;
	}
}

int PhysicsWorld::FindPlatform(const ion::Vector2i& position, int maxSearchLength) const
{
	for (int i = 0; i < m_platforms.size(); i++)
	{
		const Platform& platform = *m_platforms[i];

		//If point between platform left and right
		if (position.x >= platform.position.x && position.x < (platform.position.x + platform.width))
		{
			//If platform between point and search dist
			if (platform.position.y >= position.y && platform.position.y < (position.y + maxSearchLength))
			{
				return platform.position.y;
			}
		}
	}

	return -1;
}

int PhysicsWorld::FindWall(const ion::Vector2i& position, int direction, int maxSearchLength) const
{
	//Position to starting tile
	ion::Vector2i tilePos(position.x / 8, position.y / 8);
	u32 flags = 0;

	if (tilePos.y >= 0 && tilePos.y < m_collisionMap.GetHeight())
	{
		flags = m_collisionMap.GetCollisionTileFlags(tilePos.x, tilePos.y);
		int lengthSearched = 0;

		while ((flags & eCollisionTileFlagSolid) == 0 && lengthSearched < maxSearchLength)
		{
			if (tilePos.x >= 0 && tilePos.x < m_collisionMap.GetWidth())
			{
				tilePos.x += direction;
				flags = m_collisionMap.GetCollisionTileFlags(tilePos.x, tilePos.y);
			}

			lengthSearched += 8;
		}
	}

	if ((flags & eCollisionTileFlagSolid) != 0)
	{
		return (tilePos.x * 8) + ((direction < 0) ? 8 : 0);
	}
	else
	{
		return -1;
	}
}

int PhysicsWorld::FindBarrier(const ion::Vector2i& position, int direction, int maxSearchLength, int minBarrierHeight) const
{
	int searchLeft = position.x;
	int searchRight = position.x + (maxSearchLength * direction);

	if (searchRight < searchLeft)
	{
		std::swap(searchLeft, searchRight);
	}

	for (int i = 0; i < m_barriers.size(); i++)
	{
		const Barrier& barrier = *m_barriers[i];

		if (barrier.height >= minBarrierHeight)
		{
			//If point between barrier top and bottom
			if (position.y >= barrier.position.y && position.y < (barrier.position.y + barrier.height))
			{
				//If barrier width and search width intersect
				int barrierLeft = barrier.position.x;
				int barrierRight = (barrier.position.x + barrier.width);

				if ((searchLeft <= barrierRight) && (searchRight >= barrierLeft))
				{
					if(direction < 0)
						return barrierRight;
					else
						return barrierLeft;
				}
			}
		}
	}

	return -1;
}

void PhysicsWorld::AddPushableObject(PhysicsObj& physicsObj)
{
	m_pushableObjs.push_back(&physicsObj);
}

void PhysicsWorld::RemovePushableObject(PhysicsObj& physicsObj)
{
	ion::utils::stl::FindAndRemove(m_pushableObjs, &physicsObj);
}

const std::vector<PhysicsObj*>& PhysicsWorld::GetPushableObjects() const
{
	return m_pushableObjs;
}