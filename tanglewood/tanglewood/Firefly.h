///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Firefly.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Firefly pickup
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/SpriteObj.h"

class Firefly : public SpriteObj
{
public:
	Firefly(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Firefly();

	void Pickup();
};
