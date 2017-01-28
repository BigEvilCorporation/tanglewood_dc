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

	//Setup animations
	m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", "idle");
	m_characterAnimations[(int)CharacterAnimations::Dead] = std::make_pair("dead", "dead");
	m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", "run");
	m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", "walk");
	m_characterAnimations[(int)CharacterAnimations::Jump] = std::make_pair("jump", "jump");
	m_characterAnimations[(int)CharacterAnimations::Glide] = std::make_pair("glide", "glide");
	m_characterAnimations[(int)CharacterAnimations::Push] = std::make_pair("push", "push");
	m_characterAnimations[(int)CharacterAnimations::PushHeavy] = std::make_pair("pushHeavy", "pushHeavy");
	m_characterAnimations[(int)CharacterAnimations::Fall] = std::make_pair("fall", "fall");
	m_characterAnimations[(int)CharacterAnimations::WalkToRun] = std::make_pair("walkToRun", "walkToRun");
	m_characterAnimations[(int)CharacterAnimations::WaterWade] = std::make_pair("waterWade", "waterWade");
}

Character::~Character()
{

}

void Character::Update(float deltaTime)
{
	//Update physics
	PhysicsObj::Update(deltaTime);

	//Update animation
	UpdateAnimation();

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
	m_acceleration.x = speed * Constants::Player::defaultPlayerAcceleration.x;
}

void Character::Jump()
{
	if(m_closeToFloor)
	{
		m_velocity.y = Constants::Player::defaultPlayerJumpImpuse;
		m_jumping = true;
		m_onFloor = false;
		m_closeToFloor = false;
		m_snapToFloor = false;
	}
}

void Character::SetCharacterAnimation(CharacterAnimations animation)
{
	SetAnimation(m_characterAnimations[(int)animation].first, m_characterAnimations[(int)animation].second);
}

void Character::UpdateAnimation()
{
	if(m_jumping)
	{
		SetCharacterAnimation(CharacterAnimations::Jump);
	}
	else if(!m_closeToFloor && m_velocity.y < Constants::Player::defaultFallVelocity)
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