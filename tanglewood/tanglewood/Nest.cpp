///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Flue.cpp
// Date:		27th September 2018
// Authors:		Matt Phillips
// Description:	Fuzzl nest
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Nest.h"
#include "framework/World.h"

#include <ion/core/utils/STL.h>

Nest::Nest(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Entity(world, gameObject, gameObjType, actor)
{
	m_world.AddEntity<Nest>(*this);
}

Nest::~Nest()
{
	m_world.RemoveEntity<Nest>(*this);
}
