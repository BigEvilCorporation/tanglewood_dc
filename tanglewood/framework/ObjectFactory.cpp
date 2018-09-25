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

#include "Character.h"
#include "Player.h"
#include "PhysicsObj.h"
#include "SpriteObj.h"

#include "tanglewood/Firefly.h"
#include "tanglewood/Flue.h"
#include "tanglewood/Mushroom.h"

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

	Entity* Create(const World& world, TActorMap& actors, const GameObject& gameObject, const GameObjectType& gameObjType)
	{
		Entity* entity = NULL;
		
		const std::string& typeName = gameObjType.GetName();

		//Find default actor
		Actor* actor = FindActor(actors, typeName);
		
		//Simple, but it works for now
		//TODO: allow static type registration
		if (typeName == "Nest")
		{
			entity = new Entity(world, gameObject, gameObjType, actor);
		}
		else if(	typeName == "Nymn"
				||	typeName == "Echo")
		{
			entity = new Player(world, gameObject, gameObjType, actor);
		}
		else if (typeName == "Boulder"
			||  typeName == "Fuzzl")
		{
			entity = new SpriteObj(world, gameObject, gameObjType, actor);
		}
		else if(typeName == "Firefly")
		{
			entity = new Firefly(world, gameObject, gameObjType, actor);
		}
		else if (typeName == "Flue")
		{
			entity = new Flue(world, gameObject, gameObjType);
		}
		else if (typeName == "BouncePlant")
		{
			//Actor name differs
			actor = FindActor(actors, "mushroom");
			entity = new Mushroom(world, gameObject, gameObjType, actor);
		}
		
		return entity;
	}
}
