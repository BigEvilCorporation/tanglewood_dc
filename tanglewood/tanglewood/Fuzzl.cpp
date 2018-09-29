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
#include "Nest.h"
#include "Constants.h"
#include "Animations.h"

#include "framework/World.h"

Fuzzl::Fuzzl(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Character(world, gameObject, gameObjType, actor)
{
	m_world.AddEntity<Fuzzl>(*this);

	//Physics
	m_stepHeight = Constants::Fuzzl::stepHeight;
	m_maxVelocityX = Constants::Fuzzl::maxVelocityX;
	m_maxVelocityYUp = Constants::Fuzzl::maxVelocityYUp;
	m_maxVelocityYDown = Constants::Fuzzl::maxVelocityYDown;
	m_deceleration = Constants::Fuzzl::deceleration;

	//Character
	m_allowRunning = false;

	m_colour = ColourAbility::Yellow;
	m_nest = nullptr;

	//Setup states
	m_stateMachine.AddState(new StateIdle(*this), "idle");
	m_stateMachine.AddState(new StateWatching(*this), "watching");
	m_stateMachine.AddState(new StateRolling(*this), "rolling");
	m_stateMachine.AddState(new StateNest(*this), "nest");

	//Initial state
	m_stateMachine.SetState("idle");

	//Managing character animations manually
	m_manualAnimation = true;

	//Register as pushable obj
	m_world.RegisterPushableObject(*this);
}

Fuzzl::~Fuzzl()
{
	m_world.RemoveEntity<Fuzzl>(*this);
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

bool Fuzzl::IsInNest() const
{
	return m_nest != nullptr;
}

Nest* Fuzzl::FindNest() const
{
	const std::vector<Nest*>& nests = m_world.GetEntities<Nest>();

	for (int i = 0; i < nests.size(); i++)
	{
		if (Intersects(*nests[i]))
		{
			return nests[i];
		}
	}

	return nullptr;
}

void Fuzzl::StateIdle::OnEnterState()
{
	//Set idle anim
	m_fuzzl.PlayAnimation(Animations::Fuzzl::Yellow::idle);
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
	m_fuzzl.PlayAnimation(Animations::Fuzzl::Yellow::watch);
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
	m_fuzzl.PlayAnimation(Animations::Fuzzl::Yellow::roll);
}

void Fuzzl::StateRolling::OnUpdateState(float deltaTime)
{
	//Scale anim speed based on velocity
	float animSpeed = m_fuzzl.m_velocity.x * Constants::Fuzzl::animSpeedVelocityMul;
	m_fuzzl.GetCurrentAnimation()->SetPlaybackSpeed(animSpeed);

	//Check if touching nest
	if (Nest* nest = m_fuzzl.FindNest())
	{
		m_fuzzl.m_nest = nest;
		m_stateMachine->SetState("nest");
	}

	//Check if player goes out of view distance
	if (ion::maths::IsZero(m_fuzzl.m_velocity.x))
	{
		ion::Vector2 playerCentre = m_fuzzl.m_world.GetPlayerController()->GetCentre();

		if ((playerCentre - m_fuzzl.m_worldPos).GetLength() > Constants::Fuzzl::lostDistance)
		{
			//Player far away, set idle state
			m_stateMachine->SetState("idle");
		}
	}
}

void Fuzzl::StateNest::OnEnterState()
{
	//Snap to nest centre
	ion::debug::Assert(m_fuzzl.m_nest, "Fuzzl::StateNest::OnEnterState() - No nest to enter");
	m_fuzzl.m_worldPos.x = (m_fuzzl.m_nest->m_worldPos.x + (m_fuzzl.m_nest->m_size.x / 2.0f)) - (m_fuzzl.m_size.x / 2.0f);
	m_fuzzl.m_worldPos.y = (m_fuzzl.m_nest->m_worldPos.y + (m_fuzzl.m_nest->m_size.y / 2.0f)) - (m_fuzzl.m_size.y / 2.0f);

	//Clear movement
	m_fuzzl.m_velocity = ion::Vector2();
	m_fuzzl.m_acceleration = ion::Vector2();

	//Remove as pushable object
	m_fuzzl.m_world.UnregisterPushableObject(m_fuzzl);

	//Set roll anim
	m_fuzzl.PlayAnimation(Animations::Fuzzl::Yellow::roll);
	m_fuzzl.GetCurrentAnimation()->SetStart();

	//Init bounce timer
	m_bounceTimer = Constants::Fuzzl::bounceDelay;
}

void Fuzzl::StateNest::OnUpdateState(float deltaTime)
{
	//TODO: If player within distance, and not same colour

	m_bounceTimer -= deltaTime;
	if (m_bounceTimer <= 0.0f)
	{
		m_fuzzl.AddImpulse(ion::Vector2(0.0f, Constants::Fuzzl::bounceImpulse));
		m_bounceTimer = Constants::Fuzzl::bounceDelay;
	}

	//Roll if off floor
	if (m_fuzzl.m_closeToFloor)
	{
		m_fuzzl.GetCurrentAnimation()->SetPlaybackSpeed(0.0f);
	}
	else
	{
		m_fuzzl.GetCurrentAnimation()->SetPlaybackSpeed(Constants::Fuzzl::bounceAnimSpeed);
	}
}