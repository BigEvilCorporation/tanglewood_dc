///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Firefly.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Firefly pickup
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/SpriteObj.h"

class Firefly : public SpriteObj
{
public:
	Firefly(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Firefly();

	//Update/render
	virtual void Update(float deltaTime);
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);
};
