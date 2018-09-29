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

Character::Character(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: PhysicsObj(world, gameObject, gameObjType, actor)
{
	//Setup default state
	m_stepHeight = Constants::Character::stepHeight;
	m_maxVelocityX = Constants::Character::maxVelocityXWalking;
	m_maxVelocityYUp = Constants::Character::maxVelocityYUp;
	m_maxVelocityYDown = Constants::Character::maxVelocityYDown;
	m_deceleration = Constants::Character::decelerationIdle;

	m_walktoRunVelocity = Constants::Character::walkToRunVelocity;
	m_maxVelocityXWalking = Constants::Character::maxVelocityXWalking;
	m_maxVelocityXRunning = Constants::Character::maxVelocityXRunning;

	m_allowRunning = true;
	m_running = false;
	m_jumping = false;
	m_onFloor = false;
	m_closeToFloor = false;
	m_snapToFloor = false;
	m_manualAnimation = false;

	m_walkToRunAnimTransition = false;
}

Character::~Character()
{

}

void Character::Update(float deltaTime)
{
	//Update animation
	UpdateAnimation();

	//Update physics
	PhysicsObj::Update(deltaTime);

	//If on or close to floor, stop jumping
	if(m_closeToFloor || m_onFloor)
	{
		m_jumping = false;
		m_snapToFloor = true;
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

	//Update walk/run
	bool m_prevRunning = m_running;
	m_running = (ion::maths::Abs(m_velocity.x) >= m_walktoRunVelocity);

	if (m_allowRunning)
	{
		m_maxVelocityX = m_maxVelocityXRunning;

		if (m_running != m_prevRunning)
		{
			m_walkToRunAnimTransition = true;
		}
	}
	else
	{
		m_maxVelocityX = m_maxVelocityXWalking;
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
		//TODO: Store as member
		m_acceleration.x = speed * Constants::Character::decelerationForced.x;
	}
	else
	{
		if (m_allowRunning)
		{
			//TODO: Store as member
			m_acceleration.x = speed * Constants::Character::accelerationRunning.x;
		}
		else
		{
			//TODO: Store as member
			m_acceleration.x = speed * Constants::Character::accelerationWalking.x;
		}
	}
}

void Character::Jump()
{
	if(m_closeToFloor)
	{
		//TODO: Store as member
		m_velocity.y = Constants::Character::jumpImpulse;

		m_jumping = true;
		m_onFloor = false;
		m_closeToFloor = false;
		m_snapToFloor = false;
	}
}

void Character::SetCharacterAnimation(CharacterAnimations animation, bool interrupt)
{
	if (!m_characterAnimations[(int)animation].first.empty())
	{
		if (interrupt)
		{
			PlayAnimation(m_characterAnimations[(int)animation].second);
		}
		else
		{
			QueueAnimation(m_characterAnimations[(int)animation].second);
		}
	}
}

void Character::UpdateAnimation()
{
	if (!m_manualAnimation)
	{
		SpriteAnimation* currentAnim = GetCurrentAnimation();

		//Don't interrupt non-looping anims
		if (!currentAnim || (currentAnim->GetState() == ion::render::Animation::eStopped) || (currentAnim->GetPlaybackBehaviour() == ion::render::Animation::eLoop))
		{
			if (m_jumping)
			{
				SetCharacterAnimation(CharacterAnimations::Jump);
			}
			else if (!m_closeToFloor && m_velocity.y < -Constants::Character::fallVelocity)	//TODO: Store as member
			{
				SetCharacterAnimation(CharacterAnimations::Fall);
			}
			else if (m_pushingLight)
			{
				SetCharacterAnimation(CharacterAnimations::PushLight);
			}
			else if (m_pushingHeavy)
			{
				SetCharacterAnimation(CharacterAnimations::PushHeavy);
			}
			else if (m_closeToFloor && m_velocity.GetLength() == 0.0f)
			{
				SetCharacterAnimation(CharacterAnimations::Idle);
			}
			else if (m_closeToFloor && m_velocity.GetLength() != 0.0f)
			{
				if (m_walkToRunAnimTransition)
				{
					SetCharacterAnimation(CharacterAnimations::WalkToRun);
					m_walkToRunAnimTransition = false;
				}
				else if (m_running)
				{
					SetCharacterAnimation(CharacterAnimations::Run);
				}
				else
				{
					SetCharacterAnimation(CharacterAnimations::Walk);
				}
			}
		}
	}

	if (const AnimType* animType = GetCurrentAnimType())
	{
		if (animType->flags & AnimFlags::FreezeMovementX)
		{
			m_acceleration.x = 0.0f;
			m_velocity.x = 0.0f;
		}

		if (animType->flags & AnimFlags::FreezeMovementY)
		{
			m_acceleration.y = 0.0f;
			m_velocity.y = 0.0f;
		}
	}
}