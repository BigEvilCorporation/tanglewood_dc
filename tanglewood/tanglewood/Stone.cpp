///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Stone.cpp
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	Blocking stone
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Stone.h"

#include "framework/World.h"

Stone::Stone(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	m_barrier.position = m_worldPos;
	m_barrier.width = m_size.x;
	m_barrier.height = m_size.y;

	m_world.GetPhysicsWorld().AddBarrier(m_barrier);
}

Stone::~Stone()
{
	m_world.GetPhysicsWorld().RemoveBarrier(m_barrier);
}
