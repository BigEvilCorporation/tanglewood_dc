///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Fuzzl.cpp
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Fuzzl
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Fuzzl.h"
#include "Constants.h"

#include "framework/World.h"

Fuzzl::Fuzzl(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Character(world, gameObject, gameObjType, actor)
{
	//Setup states
	m_stateMachine.AddState(new StateIdle(*this), "idle");
	m_stateMachine.AddState(new StateWatching(*this), "watching");
	m_stateMachine.AddState(new StateRolling(*this), "rolling");

	//Initial state
	m_stateMachine.SetState("idle");

	//Managing character animations manually
	m_manualAnimation = true;

	//Register as pushable obj
	m_world.RegisterPushableObject(*this);
}

Fuzzl::~Fuzzl()
{
	m_world.UnregisterPushableObject(*this);
}

void Fuzzl::Update(float deltaTime)
{
	m_stateMachine.Update(deltaTime);
	Character::Update(deltaTime);
}

void Fuzzl::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	Character::Render(renderer, cameraInv, mapSize);
}

void Fuzzl::StateIdle::OnEnterState()
{
	//Set idle anim
	m_fuzzl.SetAnimation("yellow_idle", "yellow_idle", true);
}

void Fuzzl::StateIdle::OnUpdateState(float deltaTime)
{
	//Watch for player
	ion::Vector2 playerCentre = m_fuzzl.m_world.GetPlayerController()->GetCentre();

	if ((playerCentre - m_fuzzl.m_worldPos).GetLength() < Constants::Fuzzl::alertDistance)
	{
		//Player nearby, set watching state
		m_stateMachine->SetState("watching");
	}
}

void Fuzzl::StateWatching::OnEnterState()
{
	//Set watch anim
	m_fuzzl.SetAnimation("yellow_eyes", "yellow_eyes", true);
	m_fuzzl.GetCurrentAnimation()->SetPlaybackSpeed(0.0f);
	m_fuzzl.GetCurrentAnimation()->SetFrame(Constants::Fuzzl::eyeWatchCentreFrame);

	//Hello!
	m_fuzzl.AddImpulse(ion::Vector2(0.0f, Constants::Fuzzl::startledImpulse));
}

void Fuzzl::StateWatching::OnUpdateState(float deltaTime)
{
	//Update anim frame to point eyes towards player
	ion::Vector2 playerCentre = m_fuzzl.m_world.GetPlayerController()->GetCentre();
	ion::Vector2 fuzzlCentre = m_fuzzl.GetWorldCentre();
	ion::Vector2 direction = (playerCentre - fuzzlCentre).Normalise();
	float angleRad = direction.Angle(ion::Vector2(0.0f, 1.0f));
	float angleDeg = 360.0f - (180.0f + ion::maths::RadiansToDegrees(angleRad));
	float rotation = (360.0f / (float)Constants::Fuzzl::maxEyeWatchFrames / 2.0f);
	float angleRotated = angleDeg - rotation;
	float frame = ion::maths::Round((angleDeg / 360.0f) * (float)Constants::Fuzzl::maxEyeWatchFrames) + 0.5f;
	if (frame > Constants::Fuzzl::maxEyeWatchFrames)
		frame -= Constants::Fuzzl::maxEyeWatchFrames;
	m_fuzzl.GetCurrentAnimation()->SetFrame(frame);

	//Check if player goes out of view distance
	if ((playerCentre - m_fuzzl.m_worldPos).GetLength() > Constants::Fuzzl::lostDistance)
	{
		//Player far away, set idle state
		m_stateMachine->SetState("idle");
	}

	//Check if being rolled
	if (m_fuzzl.m_velocity.x != 0.0f)
	{
		//Moving, enter rolling state
		m_stateMachine->SetState("rolling");
	}
}

void Fuzzl::StateRolling::OnEnterState()
{
	//Set watch anim
	m_fuzzl.SetAnimation("yellow_roll", "yellow_roll", true);
}

void Fuzzl::StateRolling::OnUpdateState(float deltaTime)
{
	//Scale anim speed based on velocity
	float animSpeed = m_fuzzl.m_velocity.x / Constants::Fuzzl::animSpeedVelocityDiv;
	m_fuzzl.GetCurrentAnimation()->SetPlaybackSpeed(animSpeed);

	//Check if player goes out of view distance
	ion::Vector2 playerCentre = m_fuzzl.m_world.GetPlayerController()->GetCentre();

	if ((playerCentre - m_fuzzl.m_worldPos).GetLength() > Constants::Fuzzl::lostDistance)
	{
		//Player far away, set idle state
		m_stateMachine->SetState("idle");
	}
}