///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Enemy.h
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Enemy base behaviour
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Character.h"

class Enemy : public Character
{
public:
	Enemy(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Enemy();
};