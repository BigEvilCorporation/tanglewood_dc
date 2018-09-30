///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsWorld.cpp
// Date:		30th September 2018
// Authors:		Matt Phillips
// Description:	Physics world
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Physics.h"
#include "Constants.h"
#include "PhysicsObj.h"

#include <ion/core/utils/STL.h>

PhysicsWorld::PhysicsWorld()
{
	m_gravity = Constants::World::defaultGravity;
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

void PhysicsWorld::Step(float deltaTime)
{
	for (int i = 0; i < m_physicsObjs.size(); i++)
	{
		m_physicsObjs[i]->PhysicsStep(deltaTime, *this);
	}
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