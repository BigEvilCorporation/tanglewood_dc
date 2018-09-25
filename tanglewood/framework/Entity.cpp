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

#include <ion/maths/Geometry.h>

Entity::Entity(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: m_world(world)
	, m_gameObjType(gameObjType)
{
	m_active = true;

	m_name = gameObject.GetName();
	m_size.x = (float)gameObjType.GetDimensions().x;
	m_size.y = (float)gameObjType.GetDimensions().y;
	m_worldPos.x = (float)gameObject.GetPosition().x;
	m_worldPos.y = (float)gameObject.GetPosition().y;

	m_boundsSize = m_size;
	m_boundsBottomRight = m_size;
}

Entity::~Entity()
{

}

void Entity::GetWorldBounds(ion::Vector2& topLeft, ion::Vector2& bottomRight) const
{
	topLeft = m_worldPos + m_boundsTopLeft;
	bottomRight = m_worldPos + m_boundsBottomRight;
}

bool Entity::Intersects(const Entity& objectB) const
{
	ion::Vector2 topLeftA;
	ion::Vector2 bottomRightA;
	ion::Vector2 topLeftB;
	ion::Vector2 bottomRightB;

	GetWorldBounds(topLeftA, bottomRightA);
	objectB.GetWorldBounds(topLeftB, bottomRightB);

	return ion::maths::BoxIntersectsBox(topLeftA, bottomRightA, topLeftB, bottomRightB);
}

bool Entity::Contains(const Entity& objectB) const
{
	ion::Vector2 topLeftA;
	ion::Vector2 bottomRightA;
	ion::Vector2 topLeftB;
	ion::Vector2 bottomRightB;

	GetWorldBounds(topLeftA, bottomRightA);
	objectB.GetWorldBounds(topLeftB, bottomRightB);

	return ion::maths::BoxInsideBox(topLeftA, bottomRightA, topLeftB, bottomRightB);
}