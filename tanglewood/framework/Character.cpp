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
	m_maxVelocityXAir = Constants::Character::maxVelocityXInAir;
	m_maxVelocityYUp = Constants::Character::maxVelocityYUp;
	m_maxVelocityYDown = Constants::Character::maxVelocityYDown;
	m_deceleration = Constants::Character::decelerationIdle;

	m_walktoRunVelocity = Constants::Character::walkToRunVelocity;
	m_maxVelocityXWalking = Constants::Character::maxVelocityXWalking;
	m_maxVelocityXRunning = Constants::Character::maxVelocityXRunning;
	m_accelerationWalking = Constants::Character::accelerationWalking;
	m_accelerationRunning = Constants::Character::accelerationRunning;
	m_decelerationIdle = Constants::Character::decelerationIdle;
	m_decelerationForced = Constants::Character::decelerationForced;

	m_jumpVelY = Constants::Character::jumpImpulse;
	m_jumpVelScaleX = 1.0f;

	m_alive = true;
	m_allowRunning = true;
	m_running = false;
	m_jumping = false;
	m_onFloor = false;
	m_closeToFloor = false;
	m_snapToFloor = false;
	m_manualAnimation = false;
	m_controlEnabled = true;
	m_pushingLight = false;
	m_pushingHeavy = false;
	m_walkToRunAnimTransition = false;
	m_lockFlipDirection = false;

	SetCharacterAnimation(CharacterAnimations::Idle);
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
	if (!m_lockFlipDirection)
	{
		if (m_velocity.x < 0.0f)
		{
			m_flippedX = true;
		}
		else if (m_velocity.x > 0.0f)
		{
			m_flippedX = false;
		}
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

void Character::Move(float speed)
{
	if ((m_velocity.x < 0.0f && speed > 0.0f) || (m_velocity.x > 0.0f && speed < 0.0f))
	{
		//Character is already moving, force in other direction
		m_acceleration.x = speed * m_decelerationForced.x;
	}
	else
	{
		if (m_allowRunning)
		{
			m_acceleration.x = speed * m_accelerationRunning.x;
		}
		else
		{
			m_acceleration.x = speed * m_accelerationWalking.x;
		}
	}
}

void Character::Jump()
{
	if(m_closeToFloor && m_velocity.y <= 0.0f)
	{
		m_velocity.y = m_jumpVelY;
		m_velocity.x *= m_jumpVelScaleX;

		m_jumping = true;
		m_onFloor = false;
		m_closeToFloor = false;
		m_snapToFloor = false;
	}
}

void Character::CancelJump()
{
	if (m_jumping)
	{
		//If within jump cancellable velocity window, reset velocity
		if (m_velocity.y > Constants::Character::jumpCancelVelMin && m_velocity.y < Constants::Character::jumpCancelVelMax)
		{
			m_velocity.y = Constants::Character::jumpCancelVelMin;
		}
	}
}

void Character::Kill()
{
	SetCharacterAnimation(CharacterAnimations::Dead, true);
	m_alive = false;
	m_controlEnabled = false;
	m_acceleration.x = 0.0f;
	m_velocity.x = 0.0f;
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
	if (!m_manualAnimation && m_alive)
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
}