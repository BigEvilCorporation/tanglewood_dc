///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		ObjectFactory.cpp
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Creation of objects from Beehive type name
///////////////////////////////////////////////////////////////

#include "ObjectFactory.h"

#include "Character.h"
#include "PhysicsObj.h"
#include "SpriteObj.h"

namespace ObjectFactory
{
    Entity* Create(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
    {
        Entity* entity = NULL;
        
        const std::string& typeName = gameObjType.GetName();
        
        //Simple, but it works for now
        if(     typeName == "Nymn"
           ||   typeName == "Echo")
        {
            entity = new Character(world, gameObject, gameObjType);
        }
        else if (typeName == "Boulder"
            ||  typeName == "Fuzzl"
            ||  typeName == "BouncePlant"
            ||  typeName == "Nest")
        {
            entity = new SpriteObj(world, gameObject, gameObjType);
        }
        
        if(entity && actor)
        {
            entity->LoadActor(*actor);
        }
        
        return entity;
    }
}
