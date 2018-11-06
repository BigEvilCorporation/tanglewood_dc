///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Scirus.cpp
// Date:		6th November 2018
// Authors:		Matt Phillips
// Description:	Scirus enemy
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Scirus.h"
#include "Animations.h"
#include "Globals.h"
#include "Constants.h"
#include "framework/World.h"
#include "tanglewood/Player.h"

Scirus::Scirus(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Enemy(world, gameObject, gameObjType, actor)
{
	world.AddEntity<Scirus>(*this);

	//Exported wrong way around
	m_invertFlipX = true;
	m_flippedX = true;

	//Setup animations
	m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", Animations::Scirus::Friendly::idle);

	//Add states
	m_stateMachine.AddState(new StateIdle(*this), "idle");
	m_stateMachine.AddState(new StateFriendly(*this), "friendly");
	m_stateMachine.AddState(new StateHostile(*this), "hostile");

	//Set initial state
	m_stateMachine.SetState("idle");
}

Scirus::~Scirus()
{
	m_world.RemoveEntity<Scirus>(*this);
}

void Scirus::Update(float deltaTime)
{
	Enemy::Update(deltaTime);

	if (m_alive)
	{
		m_stateMachine.Update(deltaTime);
	}
}

void Scirus::StateIdle::OnEnterState()
{

}

void Scirus::StateIdle::OnUpdateState(float deltaTime)
{
	//Wait for player to come into view
	const ion::Vector2& playerPos = Globals::Players::player1->GetWorldCentre();
	const ion::Vector2& scirusPos = m_scirus.GetWorldCentre();

	if (ion::maths::Abs(playerPos.x - scirusPos.x) < Constants::Scirus::alertDistance)
	{
		m_stateMachine->SetState("friendly");
	}
}

void Scirus::StateFriendly::OnEnterState()
{
	//Set movement stats
	m_scirus.m_maxVelocityX = Constants::Scirus::Friendly::maxVelocityXWalking;
	m_scirus.m_deceleration = Constants::Scirus::Friendly::decelerationIdle;
	m_scirus.m_maxVelocityXWalking = Constants::Scirus::Friendly::maxVelocityXWalking;
	m_scirus.m_maxVelocityXRunning = Constants::Scirus::Friendly::maxVelocityXRunning;
	m_scirus.m_accelerationWalking = Constants::Scirus::Friendly::accelerationRunning;
	m_scirus.m_accelerationRunning = Constants::Scirus::Friendly::accelerationWalking;
	m_scirus.m_decelerationIdle = Constants::Scirus::Friendly::decelerationIdle;
	m_scirus.m_decelerationForced = Constants::Scirus::Friendly::decelerationForced;

	//Setup animations
	m_scirus.m_manualAnimation = false;
	m_scirus.m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", Animations::Scirus::Friendly::idle);
	m_scirus.m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", Animations::Scirus::Friendly::walk);
	m_scirus.m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", Animations::Scirus::Friendly::run);
}

void Scirus::StateFriendly::OnUpdateState(float deltaTime)
{
	//Get distance to player
	const ion::Vector2& playerPos = Globals::Players::player1->GetWorldCentre();
	const ion::Vector2& scirusPos = m_scirus.GetWorldCentre();
	const float distance = playerPos.x - scirusPos.x;

	//If backed against a wall
	if((m_scirus.CheckCollision((int)CollisionFlags::HitWallLeft) && (distance > 0.0f) && (distance < Constants::Scirus::Friendly::nervousDistance))
		|| (m_scirus.CheckCollision((int)CollisionFlags::HitWallRight) && (distance < 0.0f) && (distance > -Constants::Scirus::Friendly::nervousDistance)))
	{
		//Warn player off
		m_scirus.m_manualAnimation = true;
		m_scirus.PlayAnimation(Animations::Scirus::Friendly::hiss);
		m_scirus.m_flippedX = (distance < 0.0f);

		//If too close
		if (ion::maths::Abs(distance) < Constants::Scirus::Friendly::hostileDistance)
		{
			//Get nasty
			m_stateMachine->SetState("hostile");
		}
	}
	else
	{
		m_scirus.m_manualAnimation = false;

		//Follow player, but not too close
		const bool tooFarLeft = (distance < 0.0f) && (distance < -Constants::Scirus::Friendly::maxChaseDistance);
		const bool tooFarRight = (distance > 0.0f) && (distance > Constants::Scirus::Friendly::maxChaseDistance);
		const bool tooCloseLeft = (distance < 0.0f) && (distance > -Constants::Scirus::Friendly::minChaseDistance);
		const bool tooCloseRight = (distance > 0.0f) && (distance < Constants::Scirus::Friendly::minChaseDistance);

		if (tooFarRight || tooCloseLeft)
		{
			m_scirus.Move(1.0f);
		}
		else if (tooFarLeft || tooCloseRight)
		{
			m_scirus.Move(-1.0f);
		}
		else
		{
			m_scirus.Move(0.0f);
		}
	}
}

void Scirus::StateHostile::OnEnterState()
{
	//Set movement stats
	m_scirus.m_maxVelocityX = Constants::Scirus::Hostile::maxVelocityXWalking;
	m_scirus.m_deceleration = Constants::Scirus::Hostile::decelerationIdle;
	m_scirus.m_maxVelocityXWalking = Constants::Scirus::Hostile::maxVelocityXWalking;
	m_scirus.m_maxVelocityXRunning = Constants::Scirus::Hostile::maxVelocityXRunning;
	m_scirus.m_accelerationWalking = Constants::Scirus::Hostile::accelerationRunning;
	m_scirus.m_accelerationRunning = Constants::Scirus::Hostile::accelerationWalking;
	m_scirus.m_decelerationIdle = Constants::Scirus::Hostile::decelerationIdle;
	m_scirus.m_decelerationForced = Constants::Scirus::Hostile::decelerationForced;

	//Setup animations
	m_scirus.m_manualAnimation = false;
	m_scirus.m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", Animations::Scirus::Hostile::idle);
	m_scirus.m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", Animations::Scirus::Hostile::walk);
	m_scirus.m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", Animations::Scirus::Hostile::run);

	//Play transition animation
	m_scirus.PlayAnimation(Animations::Scirus::Hostile::hiss);
}

void Scirus::StateHostile::OnUpdateState(float deltaTime)
{
	if (Globals::Players::player1->m_alive)
	{
		//If touching player, kill
		if (m_scirus.Intersects(*Globals::Players::player1))
		{
			m_scirus.PlayAnimation(Animations::Scirus::Hostile::attack);
			Globals::Players::player1->Kill();
		}

		//Run towards player
		const ion::Vector2& playerPos = Globals::Players::player1->GetWorldCentre();
		const ion::Vector2& scirusPos = m_scirus.GetWorldCentre();
		const float distance = playerPos.x - scirusPos.x;

		if (playerPos.x > (scirusPos.x + Constants::Scirus::Hostile::minChaseDistance))
		{
			m_scirus.Move(1.0f);
		}
		else if (playerPos.x < (scirusPos.x - Constants::Scirus::Hostile::minChaseDistance))
		{
			m_scirus.Move(-1.0f);
		}
		else
		{
			m_scirus.Move(0.0f);
		}

		//If too far away, return to friendly state
		if (ion::maths::Abs(distance) > Constants::Scirus::Hostile::maxChaseDistance)
		{
			m_stateMachine->SetState("friendly");
		}
	}
}