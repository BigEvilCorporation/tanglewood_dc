///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Character.cpp
// Date:		15th January 2017
// Authors:		Matt Phillips
// Description:	Base for all characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Character.h"
#include "Constants.h"

Character::Character(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: PhysicsObj(world, gameObject, gameObjType, actor)
{
	//Setup default state
	m_stepHeight = Constants::Player::defaultStepHeight;

	m_jumping = false;
	m_onFloor = false;
	m_closeToFloor = false;
	m_snapToFloor = false;
	m_manualAnimation = false;
}

Character::~Character()
{

}

void Character::Update(float deltaTime)
{
	//Update physics
	PhysicsObj::Update(deltaTime);

	//Update animation
	if (!m_manualAnimation)
	{
		UpdateAnimation();
	}

	//If on or close to floor, stop jumping
	if(m_closeToFloor || m_onFloor)
	{
		m_jumping = false;

		//If running, snap to floor
		//if()
		{
			m_snapToFloor = true;
		}
	}

	//Flip sprite
	if(m_velocity.x < 0.0f)
	{
		m_flippedX = true;
	}
	else if(m_velocity.x > 0.0f)
	{
		m_flippedX = false;
	}
}

void Character::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	PhysicsObj::Render(renderer, cameraInv, mapSize);
}

void Character::Move(float speed)
{
	if ((m_velocity.x < 0.0f && speed > 0.0f) || (m_velocity.x > 0.0f && speed < 0.0f))
	{
		m_acceleration.x = speed * Constants::Player::defaultPlayerDecelerationForced.x;
	}
	else
	{
		m_acceleration.x = speed * Constants::Player::defaultPlayerAcceleration.x;
	}
}

void Character::Jump()
{
	if(m_closeToFloor)
	{
		m_velocity.y = Constants::Player::defaultPlayerJumpImpulse;
		m_jumping = true;
		m_onFloor = false;
		m_closeToFloor = false;
		m_snapToFloor = false;
	}
}

void Character::SetCharacterAnimation(CharacterAnimations animation)
{
	//TODO: non-looping anims
	if (!m_characterAnimations[(int)animation].first.empty())
	{
		SetAnimation(m_characterAnimations[(int)animation].first, m_characterAnimations[(int)animation].second, true);
	}
}

void Character::UpdateAnimation()
{
	if(m_jumping)
	{
		SetCharacterAnimation(CharacterAnimations::Jump);
	}
	else if(!m_closeToFloor && m_velocity.y < -Constants::Player::defaultFallVelocity)
	{
		SetCharacterAnimation(CharacterAnimations::Fall);
	}
	else if(m_closeToFloor && m_velocity.GetLength() == 0.0f)
	{
		SetCharacterAnimation(CharacterAnimations::Idle);
	}
	else if(m_closeToFloor && m_velocity.GetLength() != 0.0f)
	{
		SetCharacterAnimation(CharacterAnimations::Run);
	}
}