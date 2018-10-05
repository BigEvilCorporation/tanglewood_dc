///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Stone.h
// Date:		5th October 2018
// Authors:		Matt Phillips
// Description:	Blocking stone
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/SpriteObj.h"
#include "framework/Barrier.h"

class Stone : public SpriteObj
{
public:
	Stone(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Stone();

private:
	Barrier m_barrier;
};
