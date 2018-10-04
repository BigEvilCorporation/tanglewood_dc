///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Hogg.cpp
// Date:		4th October 2018
// Authors:		Matt Phillips
// Description:	Hogg enemy
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Hogg.h"
#include "Animations.h"
#include "Globals.h"
#include "Constants.h"
#include "framework/World.h"
#include "tanglewood/Player.h"

Hogg::Hogg(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Enemy(world, gameObject, gameObjType, actor)
{
	world.AddEntity<Hogg>(*this);

	m_maxVelocityX = Constants::Hogg::maxVelocityXWalking;
	m_deceleration = Constants::Hogg::decelerationIdle;

	m_maxVelocityXWalking = Constants::Hogg::maxVelocityXWalking;
	m_maxVelocityXRunning = Constants::Hogg::maxVelocityXRunning;
	m_accelerationWalking = Constants::Hogg::accelerationRunning;
	m_accelerationRunning = Constants::Hogg::accelerationWalking;
	m_decelerationIdle = Constants::Hogg::decelerationIdle;
	m_decelerationForced = Constants::Hogg::decelerationForced;

	//Setup animations
	m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", Animations::Hogg::idle);
	m_characterAnimations[(int)CharacterAnimations::Dead] = std::make_pair("dead", Animations::Hogg::dead);
	m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", Animations::Hogg::run);
	m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", Animations::Hogg::run);

	//Add states
	m_stateMachine.AddState(new StateIdle(*this), "idle");
	m_stateMachine.AddState(new StateChase(*this), "chase");
	m_stateMachine.AddState(new StateStuck(*this), "stuck");

	//Set initial state
	m_stateMachine.SetState("idle");
}

Hogg::~Hogg()
{
	m_world.RemoveEntity<Hogg>(*this);
}

void Hogg::Update(float deltaTime)
{
	Enemy::Update(deltaTime);

	if (m_alive)
	{
		m_stateMachine.Update(deltaTime);
	}
}

void Hogg::StateIdle::OnEnterState()
{

}

void Hogg::StateIdle::OnUpdateState(float deltaTime)
{
	//Wait for player to come into view
	const ion::Vector2& playerPos = Globals::Players::player1->GetWorldCentre();
	const ion::Vector2& hoggPos = m_hogg.GetWorldCentre();

	if (ion::maths::Abs(playerPos.x - hoggPos.x) < Constants::Hogg::alertDistance)
	{
		m_stateMachine->SetState("chase");
	}
}

void Hogg::StateChase::OnEnterState()
{
	//Relinquish animation control
	m_hogg.m_manualAnimation = false;

	//Reset movement
	m_hogg.m_velocity = ion::Vector2();
	m_hogg.m_acceleration = ion::Vector2();
}

void Hogg::StateChase::OnUpdateState(float deltaTime)
{
	//If hit wall, get stuck
	if (ion::maths::Abs(m_hogg.m_lastWallVelocity) > Constants::Hogg::stuckVelocity)
	{
		m_stateMachine->SetState("stuck");
	}
	else
	{
		//If touching player, kill
		if (Globals::Players::player1->m_alive && m_hogg.Intersects(*Globals::Players::player1))
		{
			Globals::Players::player1->Kill();
		}

		//Run towards player
		const ion::Vector2& playerPos = Globals::Players::player1->GetWorldCentre();
		const ion::Vector2& hoggPos = m_hogg.GetWorldCentre();

		if (playerPos.x > (hoggPos.x + Constants::Hogg::minChaseDistance))
		{
			m_hogg.Move(1.0f);
		}
		else if (playerPos.x < (hoggPos.x - Constants::Hogg::minChaseDistance))
		{
			m_hogg.Move(-1.0f);
		}
		else
		{
			m_hogg.Move(0.0f);
		}
	}
}

void Hogg::StateStuck::OnEnterState()
{
	//Take animation control
	m_hogg.m_manualAnimation = true;

	//Play hit wall animation, queue stuck
	m_hogg.PlayAnimation(Animations::Hogg::wall);
	m_hogg.QueueAnimation(Animations::Hogg::stuck);

	//Set timer
	m_stuckTimer = Constants::Hogg::stuckTime;
}

void Hogg::StateStuck::OnUpdateState(float deltaTime)
{
	if (m_stuckTimer > 0.0f)
	{
		m_stuckTimer -= deltaTime;
	}
	else
	{
		//Set unstuck animation
		m_hogg.PlayAnimation(Animations::Hogg::unstuck);

		//Impulse out of wall
		m_hogg.AddImpulse(ion::Vector2(m_hogg.m_flippedX ? Constants::Hogg::stuckExitImpulse : -Constants::Hogg::stuckExitImpulse, 0.0f));

		//Back to chase state
		m_stateMachine->SetState("chase");
	}
}