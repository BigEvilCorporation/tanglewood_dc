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

	//If velocity > tile size, time slice it
	int numTimeSteps = 1;
	ion::Vector2 velocitySlice = m_velocity;
	float timeSlice = deltaTime;
	float searchDist = s_floorSearchDist;
	const float minSearchDist = 16.0f;

	float velocitySize = m_velocity.GetLength() * deltaTime;
	if(velocitySize > 8.0f)
	{
		numTimeSteps = ion::maths::Ceil(velocitySize / 8.0f);
		velocitySlice = m_velocity / (float)numTimeSteps;
		timeSlice = deltaTime / (float)numTimeSteps;
		searchDist = s_floorSearchDist / (float)numTimeSteps;
	}

	for(int i = 0; i < numTimeSteps; i++)
	{
		//Apply velocity
		m_worldPos.x += velocitySlice.x * timeSlice;
		m_worldPos.y -= velocitySlice.y * timeSlice;

		//Find wall
		ion::Vector2i wallProbe((int)(m_worldPos.x + ((velocitySlice.x > 0.0f) ? m_size.x : 0.0f)), (int)(m_worldPos.y + m_floorProbeOffset.y));

		float wallPos = (float)m_world.FindWall(wallProbe, velocitySlice.x > 0.0f ? 1 : -1, (int)ion::maths::Max(searchDist, minSearchDist));

		if(wallPos >= 0.0f && velocitySlice.x > 0.0f && wallPos < (m_worldPos.x + m_size.x))
		{
			//Collision with wall to right, adjust position
			m_worldPos.x = wallPos - m_size.x;

			//Kill X velocity/acceleration
			m_velocity.x = 0.0f;
			m_acceleration.x = 0.0f;
			velocitySlice.x = 0.0f;
		}
		else if(wallPos >= 0.0f && velocitySlice.x < 0.0f && wallPos > m_worldPos.x)
		{
			//Collision with wall to left, adjust position
			m_worldPos.x = wallPos;

			//Kill X velocity/acceleration
			m_velocity.x = 0.0f;
			m_acceleration.x = 0.0f;
			velocitySlice.x = 0.0f;
		}

		//Find floor
		ion::Vector2 floorProbe(m_worldPos.x + m_floorProbeOffset.x, m_worldPos.y + m_floorProbeOffset.y);

		u16 floorFlags = 0;
		float floorHeight = (float)m_world.FindFloor(ion::Vector2i((int)floorProbe.x, (int)floorProbe.y), (int)ion::maths::Max(searchDist, minSearchDist), floorFlags);

		if(floorHeight >= 0.0f && floorHeight < floorProbe.y)
		{
			//Collision with floor, adjust position
			m_worldPos.y = floorHeight - m_floorProbeOffset.y;

			//Kill downward velocity
			m_velocity.y = 0.0f;
			velocitySlice.y = 0.0f;
		}
	}

	return SpriteObj::Update(deltaTime);
}

void PhysicsObj::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	return SpriteObj::Render(renderer, cameraInv, mapSize);
}