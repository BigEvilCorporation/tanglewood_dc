///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		ObjectFactory.h
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Creation of objects from Beehive type name
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/beehive/Project.h>
#include <ion/beehive/Actor.h>

#include "Entity.h"

namespace ObjectFactory
{
	Entity* Create(World& world, TActorMap& actors, const GameObject& gameObject, const GameObjectType& gameObjType);
}
