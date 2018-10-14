///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsWorld.h
// Date:		30th September 2018
// Authors:		Matt Phillips
// Description:	Physics world
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Barrier.h"
#include "Platform.h"

#include <ion/beehive/TerrainTileset.h>
#include <ion/beehive/CollisionMap.h>

class PhysicsObj;
class Project;

class PhysicsWorld
{
public:
	PhysicsWorld();

	const std::vector<PhysicsObj*>& GetPhysicsObjs() const { return m_physicsObjs; }

	void AddObject(PhysicsObj& physicsObj);
	void RemoveObject(PhysicsObj& physicsObj);

	void AddPlatform(Platform& platform);
	void RemovePlatform(Platform& platform);

	void AddBarrier(Barrier& barrier);
	void RemoveBarrier(Barrier& barrier);

	void AddPushableObject(PhysicsObj& physicsObj);
	void RemovePushableObject(PhysicsObj& physicsObj);
	const std::vector<PhysicsObj*>& GetPushableObjects() const;

	float GetGravity() const { return m_gravity; }

	//Load physics world
	void LoadWorld(Project& project, const std::string& levelMap);

	//Reset physics world
	void RemoveAllObjects();

	//Step physics world
	void Step(float deltaTime);

	//Collision queries
	int FindFloor(const ion::Vector2i& position, int maxSearchLength, u16& tileFlags) const;
	int FindPlatform(const ion::Vector2i& position, int maxSearchLength) const;
	int FindWall(const ion::Vector2i& position, int direction, int maxSearchLength) const;
	int FindBarrier(const ion::Vector2i& position, int direction, int maxSearchLength, int minBarrierHeight) const;

	//Global speed scale
	void SetGlobalSpeedScale(float scale) { m_speedScale = scale; }
	float GetGlobalSpeedScale() const { return m_speedScale; }

private:

	//Physics data
	TerrainTileset m_terrainTileset;
	CollisionMap m_collisionMap;

	//Gravity
	float m_gravity;

	//Speed scale
	float m_speedScale;

	std::vector<PhysicsObj*> m_physicsObjs;
	std::vector<PhysicsObj*> m_pushableObjs;
	std::vector<Platform*> m_platforms;
	std::vector<Barrier*> m_barriers;
};