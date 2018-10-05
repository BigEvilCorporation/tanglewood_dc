///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		ObjectFactory.cpp
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Creation of objects from Beehive type name
///////////////////////////////////////////////////////////////

#include "ObjectFactory.h"

#include <ion/core/string/String.h>
#include <ion/core/debug/Debug.h>

#include <algorithm>

namespace ObjectFactory
{
	Actor* FindActor(TActorMap& actors, const std::string& actorName)
	{
		//Find actor in sprite data
		Actor* actor = nullptr;

		for (TActorMap::iterator it = actors.begin(), end = actors.end(); it != end && !actor; ++it)
		{
			if (ion::string::CompareNoCase(it->second.GetName(), actorName))
			{
				actor = &it->second;
			}
		}

		return actor;
	}

	Entity* Create(World& world, TActorMap& actors, const GameObject& gameObject, const GameObjectType& gameObjType)
	{
		Entity* entity = NULL;
		
		const std::string& typeName = gameObjType.GetName();

		//Find in registry
		std::vector<ObjectRegistryEntry>::const_iterator it = std::find_if(objectRegistry.begin(), objectRegistry.end(), [&typeName](const ObjectRegistryEntry& rhs) { return typeName == rhs.typeName; });

		if (it != objectRegistry.end())
		{
			const ObjectRegistryEntry& registryEntry = (*it);

			//Find actor
			Actor* actor = nullptr;
			if (!registryEntry.actorName.empty())
			{
				//Find actor
				actor = FindActor(actors, registryEntry.actorName);

				if (!actor)
				{
					ion::debug::log << "ObjectFactory::Create() - Could not find sprite actor \'" << registryEntry.actorName << "\'" << ion::debug::end;
				}
			}

			//Create!
			entity = registryEntry.allocator(world, gameObject, gameObjType, actor);
		}
		else
		{
			ion::debug::log << "ObjectFactory::Create() - Could not find object factory for type \'" << typeName << "\'" << ion::debug::end;
		}

		return entity;
	}
}
