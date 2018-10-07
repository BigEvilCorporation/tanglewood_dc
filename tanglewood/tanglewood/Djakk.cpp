///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Djakk.cpp
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Djakk enemy
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Djakk.h"
#include "Animations.h"
#include "Globals.h"
#include "Constants.h"
#include "framework/World.h"
#include "tanglewood/Player.h"

Djakk::Djakk(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Enemy(world, gameObject, gameObjType, actor)
{
	world.AddEntity<Djakk>(*this);

	m_maxVelocityX = Constants::Djakk::maxVelocityXWalking;
	m_deceleration = Constants::Djakk::decelerationIdle;

	m_walktoRunVelocity = Constants::Djakk::walkToRunVelocity;
	m_maxVelocityXWalking = Constants::Djakk::maxVelocityXWalking;
	m_maxVelocityXRunning = Constants::Djakk::maxVelocityXRunning;
	m_accelerationWalking = Constants::Djakk::accelerationWalking;
	m_accelerationRunning = Constants::Djakk::accelerationRunning;
	m_decelerationIdle = Constants::Djakk::decelerationIdle;
	m_decelerationForced = Constants::Djakk::decelerationForced;

	//Setup animations
	m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", Animations::Djakk::idle);
	m_characterAnimations[(int)CharacterAnimations::Dead] = std::make_pair("dead", Animations::Djakk::dead);
	m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", Animations::Djakk::run);
	m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", Animations::Djakk::walk);
	m_characterAnimations[(int)CharacterAnimations::Jump] = std::make_pair("jump", Animations::Djakk::jump);

	//Add states
	m_stateMachine.AddState(new StateIdle(*this), "idle");
	m_stateMachine.AddState(new StateSearch(*this), "search");
	m_stateMachine.AddState(new StateChase(*this), "chase");
	m_stateMachine.AddState(new StateBite(*this), "bite");

	//Set initial state
	m_stateMachine.SetState("idle");
}

Djakk::~Djakk()
{
	m_world.RemoveEntity<Djakk>(*this);
}

void Djakk::Update(float deltaTime)
{
	Enemy::Update(deltaTime);

	if (m_alive)
	{
		m_stateMachine.Update(deltaTime);
	}
}

void Djakk::BeginChase(bool roar)
{
	if (roar)
	{
		PlayAnimation(Animations::Djakk::roar);
	}

	m_stateMachine.SetState("chase");
}

void Djakk::StateIdle::OnEnterState()
{

}

void Djakk::StateIdle::OnUpdateState(float deltaTime)
{

}

void Djakk::StateSearch::OnEnterState()
{
	m_djakk.m_allowRunning = false;
	m_targetPosX = 0.0f;
	m_waitTimer = Constants::Djakk::searchRandDelayMax + (ion::maths::RandFloat() * Constants::Djakk::searchRandDelayMax);
}

void Djakk::StateSearch::OnUpdateState(float deltaTime)
{
	if (m_waitTimer > 0.0f)
	{
		m_waitTimer -= deltaTime;
	}
	else
	{
		//If no target, pick a random position
		if (m_targetPosX == 0.0f)
		{
			m_targetPosX = m_djakk.m_worldPos.x + ((ion::maths::RandFloat() - 0.5f) * Constants::Djakk::searchRandDistMax);
		}

		//Move towards target	
		if (m_djakk.m_worldPos.x > (m_targetPosX + Constants::Djakk::searchNearTargetDist))
		{
			m_djakk.Move(-1.0f);
		}
		else if (m_djakk.m_worldPos.x < (m_targetPosX - Constants::Djakk::searchNearTargetDist))
		{
			m_djakk.Move(1.0f);
		}
		else
		{
			//Reached target, stop moving and reset wait timer
			m_djakk.Move(0.0f);
			m_targetPosX = 0.0f;
			m_waitTimer = Constants::Djakk::searchRandDelayMax + (ion::maths::RandFloat() * Constants::Djakk::searchRandDelayMax);
		}
	}
}

void Djakk::StateChase::OnEnterState()
{
	m_djakk.m_allowRunning = true;
}

void Djakk::StateChase::OnUpdateState(float deltaTime)
{
	//Move towards player
	const ion::Vector2& playerPos = Globals::Players::player1->GetWorldCentre();
	const ion::Vector2& djakkPos = m_djakk.GetWorldCentre();

	if (playerPos.x > (djakkPos.x + Constants::Djakk::minChaseDistance))
	{
		m_djakk.Move(1.0f);
	}
	else if (playerPos.x < (djakkPos.x - Constants::Djakk::minChaseDistance))
	{
		m_djakk.Move(-1.0f);
	}
	else
	{
		m_djakk.Move(0.0f);
	}

	//Don't attack in time slow mode
	if (m_djakk.m_speedScale == 1.0f)
	{
		//If touching player, attack
		if (m_djakk.Intersects(*Globals::Players::player1))
		{
			m_stateMachine->SetState("bite");
		}
	}
}

void Djakk::StateBite::OnEnterState()
{
	//Begin bite
	m_djakk.PlayAnimation(Animations::Djakk::bite);
}

void Djakk::StateBite::OnUpdateState(float deltaTime)
{
	//Don't attack in time slow mode
	if (m_djakk.m_speedScale == 1.0f)
	{
		//Wait for anim to reach attack frame
		if (ion::maths::Floor(m_djakk.GetCurrentAnimation()->GetFrame()) == Constants::Djakk::biteAttackFrame)
		{
			//If still intersecting player, kill and enter search state
			if (m_djakk.Intersects(*Globals::Players::player1))
			{
				Globals::Players::player1->Kill();
				m_stateMachine->SetState("search");
			}
			else
			{
				//else back to chase state
				m_stateMachine->SetState("chase");
			}
		}
	}
}