///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Physics object and collision handling
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "PhysicsObj.h"
#include "World.h"
#include "Constants.h"

const float PhysicsObj::s_floorSearchDist = 16.0f;

PhysicsObj::PhysicsObj(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
	: SpriteObj(world, gameObject, gameObjType)
{
	m_maxVelocity = Constants::Player::defaultPlayerMaxVelocity;
	m_deceleration = Constants::Player::defaultPlayerDecelerationIdle;
	m_floorProbeOffset = ion::Vector2(m_size.x / 2.0f, m_size.y - 8.0f);
}

PhysicsObj::~PhysicsObj()
{

}

void PhysicsObj::Update(float deltaTime)
{
	//Apply acceleration
	m_velocity += m_acceleration * deltaTime;

	//Apply deceleration
	if(ion::maths::Abs(m_acceleration.x) <= ion::maths::FLOAT_EPSILON)
	{
		//Clamp
		if(m_velocity.x > 0.0f)
		{
			m_velocity.x -= m_deceleration.x * deltaTime;

			if(m_velocity.x < 0.0f)
			{
				m_velocity.x = 0.0f;
			}
		}
		else if(m_velocity.x < 0.0f)
		{
			m_velocity.x += m_deceleration.x * deltaTime;

			if(m_velocity.x > 0.0f)
			{
				m_velocity.x = 0.0f;
			}
		}
		
	}

	//Apply gravity
	m_velocity.y -= m_world.GetGravity();

	//Clamp to max velocity
	m_velocity.x = ion::maths::Clamp(m_velocity.x, -m_maxVelocity.x, m_maxVelocity.x);
	m_velocity.y = ion::maths::Clamp(m_velocity.y, -m_maxVelocity.y, m_maxVelocity.y);

	//Apply velocity
	m_worldPos.x += m_velocity.x * deltaTime;
	m_worldPos.y -= m_velocity.y * deltaTime;

	//Find floor
	ion::Vector2 floorProbe(m_worldPos + m_floorProbeOffset);

	u8 floorFlags = 0;
	float floorHeight = m_world.GetFloorHeight(floorProbe, ion::maths::Max(s_floorSearchDist, s_floorSearchDist * (ion::maths::Abs(m_velocity.y) * deltaTime)), floorFlags);

	if(floorHeight >= 0.0f && floorHeight < floorProbe.y)
	{
		//Collision with floor, adjust position
		m_worldPos.y = floorHeight - m_floorProbeOffset.y;

		//Kill downward velocity
		m_velocity.y = 0.0f;
	}

	return SpriteObj::Update(deltaTime);
}

void PhysicsObj::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	return SpriteObj::Render(renderer, cameraInv, mapSize);
}