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
#include "Constants.h"

//TODO: Move to Nymn/Echo
#include "tanglewood/Flue.h"

Player::Player(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Character(world, gameObject, gameObjType, actor)
{
	m_boundsSize.x = Constants::Player::boundsWidth;
	m_boundsSize.y = Constants::Player::boundsHeight;
	m_boundsTopLeft.x = (m_size.x / 2.0f) - (Constants::Player::boundsWidth / 2);
	m_boundsTopLeft.y = (m_size.y / 2.0f) - (Constants::Player::boundsHeight / 2);
	m_boundsBottomRight.x = m_boundsTopLeft.x + Constants::Player::boundsWidth;
	m_boundsBottomRight.y = m_boundsTopLeft.y + Constants::Player::boundsHeight;

	Flue::RegisterPotentialOccupant(*this);
}

Player::~Player()
{
	Flue::UnregisterPotentialOccupant(*this);
}

void Player::Update(float deltaTime)
{
	Character::Update(deltaTime);

	//If jumping, ignore terrain holes
	m_ignoreHoles = m_jumping;
}

void Player::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	Character::Render(renderer, cameraInv, mapSize);
}
