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

#include "Platform.h"

class PhysicsObj;

class PhysicsWorld
{
public:
	PhysicsWorld();

	void AddObject(PhysicsObj& physicsObj);
	void RemoveObject(PhysicsObj& physicsObj);

	void AddPlatform(Platform& platform);
	void RemovePlatform(Platform& platform);

	void AddPushableObject(PhysicsObj& physicsObj);
	void RemovePushableObject(PhysicsObj& physicsObj);
	const std::vector<PhysicsObj*>& GetPushableObjects() const;

	float GetGravity() const { return m_gravity; }

	void Step(float deltaTime);

private:

	//Gravity
	float m_gravity;

	std::vector<PhysicsObj*> m_physicsObjs;
	std::vector<PhysicsObj*> m_pushableObjs;
	std::vector<Platform*> m_platforms;
};