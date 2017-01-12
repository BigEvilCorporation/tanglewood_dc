///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Entity.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Basic game entity
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Entity.h"

Entity::Entity(const GameObject& gameObject, const GameObjectType& gameObjType)
{
	m_active = true;

	m_name = gameObject.GetName();
	m_size.x = (float)gameObjType.GetDimensions().x;
	m_size.y = (float)gameObjType.GetDimensions().y;
	m_worldPos.x = (float)gameObject.GetPosition().x;
	m_worldPos.y = (float)gameObject.GetPosition().y;
}

Entity::~Entity()
{

}
