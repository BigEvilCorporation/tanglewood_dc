///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Player.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Base for player characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Character.h"

class Player : public Character
{
public:
	Player(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
    virtual ~Player();
    
    //Update/render
    virtual void Update(float deltaTime);
    virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);
};
