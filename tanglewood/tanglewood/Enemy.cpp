///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Enemy.cpp
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Enemy base behaviour
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Enemy.h"

Enemy::Enemy(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Character(world, gameObject, gameObjType, actor)
{

}

Enemy::~Enemy()
{

}

void Enemy::Update(float deltaTime)
{
	Character::Update(deltaTime);
}

void Enemy::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	Character::Render(renderer, cameraInv, mapSize);
}