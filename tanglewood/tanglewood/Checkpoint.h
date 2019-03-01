///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Stone.h
// Date:		6th November 2018
// Authors:		Matt Phillips
// Description:	Checkpoint totem
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/SpriteObj.h"
#include "framework/Barrier.h"

class Checkpoint : public SpriteObj
{
public:
	Checkpoint(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Checkpoint();

	virtual void Update(float deltaTime);

private:
	bool m_activated;
};
