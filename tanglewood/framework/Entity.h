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
	Entity(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType);
	virtual ~Entity();
    
    //Load actor from Beehive data
    virtual void LoadActor(Actor& actor) {}

	//Update/render
	virtual void Update(float deltaTime) {}
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize) {}

	std::string m_name;			//Name
	ion::Vector2 m_worldPos;	//World position
	ion::Vector2 m_size;		//Game object size
	bool m_active;				//Active flag

	const World& m_world;
};
