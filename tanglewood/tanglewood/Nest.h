///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Flue.h
// Date:		27th September 2018
// Authors:		Matt Phillips
// Description:	Fuzzl nest
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Entity.h"

#include <vector>

class Nest : public Entity
{
public:
	Nest(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Nest();
};
