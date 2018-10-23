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

#include <vector>

#define REGISTER_OBJECT(name, actorName, type) name, actorName, [](World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor) { return (Entity*)new type(world, gameObject, gameObjType, actor); }

namespace ObjectFactory
{
	typedef Entity* (*ObjectAllocator)(World&, const GameObject&, const GameObjectType&, Actor*);

	struct ObjectRegistryEntry
	{
		std::string typeName;
		std::string actorName;
		ObjectAllocator allocator;
	};

	Entity* Create(World& world, std::vector<Actor>& actors, const GameObject& gameObject, const GameObjectType& gameObjType);
	Actor* FindActor(std::vector<Actor>& actors, const std::string& actorName);

	extern const std::vector<ObjectRegistryEntry> objectRegistry;
}
