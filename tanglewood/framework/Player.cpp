///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Player.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Base for player characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Player.h"

Player::Player(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
    : Character(world, gameObject, gameObjType)
{
    
}

Player::~Player()
{
    
}

void Player::Update(float deltaTime)
{
    Character::Update(deltaTime);
}

void Player::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
    Character::Render(renderer, cameraInv, mapSize);
}
