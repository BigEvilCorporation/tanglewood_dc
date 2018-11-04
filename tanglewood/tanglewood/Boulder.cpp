///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Boulder.cpp
// Date:		29th September 2018
// Authors:		Matt Phillips
// Description:	Rolling boulder
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Boulder.h"
#include "Constants.h"
#include "Globals.h"
#include "Animations.h"
#include "Djakk.h"

#include "framework/World.h"

Boulder::Boulder(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: PhysicsObj(world, gameObject, gameObjType, actor)
{
	m_spawnPos = m_worldPos;

	m_canPush = true;
	m_canPull = false;

	Respawn();
}

Boulder::~Boulder()
{
	m_world.GetPhysicsWorld().RemovePushableObject(*this);
	m_world.GetPhysicsWorld().RemovePlatform(m_platform);
}

void Boulder::Update(float deltaTime)
{
	PhysicsObj::Update(deltaTime);

	//Update platform
	m_platform.position = m_worldPos + ion::Vector2(0.0f, Constants::Boulder::platformYOffset);

	if (m_active)
	{
		//If smashed, respawn if of screen and out of respawn distance
		if (m_smashed)
		{
			if (!m_drawnLastFrame && (Globals::Game::camera->GetPosition().xy() - m_worldPos).GetLength() > Constants::Boulder::respawnDistance)
			{
				Respawn();
			}
		}
		else
		{
			//Scale anim speed based on velocity
			float animSpeed = m_velocity.x * Constants::Fuzzl::animSpeedVelocityMul;
			GetCurrentAnimation()->SetPlaybackSpeed(animSpeed);

			//If fall valocity is enough to cause damage
			if (m_velocity.y < -Constants::Boulder::minDamageYVel)
			{
				//Check if squashing enemies
				CheckSquashDjakk();
			}

			//Check if hit floor
			if (m_lastFloorVelocity < -Constants::Boulder::minDamageYVel)
			{
				Smash();
			}
		}
	}
}

void Boulder::Smash()
{
	PlayAnimation(Animations::Boulder::crack);
	m_velocity.x = 0.0f;
	m_acceleration.x = 0.0f;
	m_world.GetPhysicsWorld().RemovePushableObject(*this);
	m_world.GetPhysicsWorld().RemovePlatform(m_platform);
	m_smashed = true;
}

void Boulder::Respawn()
{
	//Reset pos
	m_worldPos = m_spawnPos;

	//Setup animation
	PlayAnimation(Animations::Boulder::roll);

	//Setup platform
	m_platform.position = m_worldPos + ion::Vector2(0.0f, Constants::Boulder::platformYOffset);
	m_platform.width = m_size.x;

	//Add as pushable
	m_world.GetPhysicsWorld().AddPushableObject(*this);

	//Add platform
	m_world.GetPhysicsWorld().AddPlatform(m_platform);

	m_smashed = false;
}

void Boulder::CheckSquashDjakk()
{
	const std::vector<Djakk*>& djakks = m_world.GetEntities<Djakk>();

	for (int i = 0; i < djakks.size() && m_active; i++)
	{
		if (djakks[i]->m_active && Intersects(*djakks[i]))
		{
			//Kill Djakk and smash boulder
			djakks[i]->Kill();
			Smash();

			//Can't respawn
			m_active = false;
		}
	}
}
