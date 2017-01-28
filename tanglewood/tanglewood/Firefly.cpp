///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Firefly.cpp
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Firefly pickup
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Firefly.h"

Firefly::Firefly(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	//Setup animation
	SetAnimation("red", "idle");
}

Firefly::~Firefly()
{

}

void Firefly::Update(float deltaTime)
{
	SpriteObj::Update(deltaTime);
}

void Firefly::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	SpriteObj::Render(renderer, cameraInv, mapSize);
}
