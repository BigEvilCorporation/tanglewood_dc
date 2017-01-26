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
        
        //Simple, but it works for now
        if(gameObjType.GetName() == "Nymn")
        {
            Character* character = new Character(world, gameObject, gameObjType);
            
            if(actor)
            {
                character->LoadActor(*actor);
            }
            
            entity = character;
        }
        
        return entity;
    }
}
