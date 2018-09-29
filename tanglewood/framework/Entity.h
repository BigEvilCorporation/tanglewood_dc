///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Entity.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Basic game entity
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Renderer.h>
#include <ion/beehive/GameObject.h>
#include <ion/beehive/Actor.h>

class World;

class Entity
{
public:
	Entity(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Entity();

	//Update/render
	virtual void Update(float deltaTime) {}
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize) {}

	ion::Vector2 GetWorldCentre() const;
	void GetWorldBounds(ion::Vector2& topLeft, ion::Vector2& bottomRight) const;

	bool Intersects(const Entity& objectB) const;
	bool Contains(const Entity& objectB) const;

	std::string m_name;				//Name
	ion::Vector2 m_worldPos;		//World position
	ion::Vector2 m_size;			//Game object size
	ion::Vector2 m_boundsSize;		//Collision bounds
	ion::Vector2 m_boundsTopLeft;
	ion::Vector2 m_boundsBottomRight;
	bool m_active;					//Active flag

	Actor* m_actor;
	const GameObjectType& m_gameObjType;
	World& m_world;
};
