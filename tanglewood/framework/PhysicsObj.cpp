///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Physics object and collision handling
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "PhysicsObj.h"

PhysicsObj::PhysicsObj(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
	: SpriteObj(world, gameObject, gameObjType)
{

}

PhysicsObj::~PhysicsObj()
{

}

void PhysicsObj::Update(float deltaTime)
{
	return SpriteObj::Update(deltaTime);
}

void PhysicsObj::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	return SpriteObj::Render(renderer, cameraInv, mapSize);
}