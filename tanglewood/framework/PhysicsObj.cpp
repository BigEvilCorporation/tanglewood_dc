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

PhysicsObj::PhysicsObj(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	m_maxVelocityX = Constants::Player::defaultPlayerMaxVelocityX;
	m_maxVelocityYUp = Constants::Player::defaultPlayerMaxVelocityYUp;
	m_maxVelocityYDown = Constants::Player::defaultPlayerMaxVelocityYDown;
	m_deceleration = Constants::Player::defaultPlayerDecelerationIdle;
	m_floorProbeOffset = ion::Vector2(m_size.x / 2.0f, m_size.y - Constants::MegaDrive::tileHeight);

	m_stepHeight = 1.0f;
	m_snapToFloor = false;

	m_ignoreHoles = false;

	m_onFloor = false;
	m_closeToFloor = false;
	m_hitWall = false;
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
	m_velocity.y -= m_world.GetGravity() * deltaTime;

	//Apply impulse
	m_velocity += m_impulse;
	m_impulse.x = 0.0f;
	m_impulse.y = 0.0f;

	//Clamp to max velocity
	m_velocity.x = ion::maths::Clamp(m_velocity.x, -m_maxVelocityX, m_maxVelocityX);
	m_velocity.y = ion::maths::Clamp(m_velocity.y, -m_maxVelocityYDown, m_maxVelocityYUp);

	//If velocity > tile size, time slice it
	int numTimeSteps = 1;
	ion::Vector2 velocitySlice = m_velocity;

	float velocitySize = m_velocity.GetLength() * deltaTime;
	if(velocitySize > Constants::MegaDrive::tileWidth)
	{
		numTimeSteps = ion::maths::Ceil(velocitySize / Constants::MegaDrive::tileWidth);
		velocitySlice = m_velocity / (float)numTimeSteps;
	}

	//Clear floor/wall flags
	m_onFloor = false;
	m_closeToFloor = false;
	m_hitWall = false;

	for(int i = 0; i < numTimeSteps; i++)
	{
		//Apply velocity
		m_worldPos.x += velocitySlice.x * deltaTime;
		m_worldPos.y -= velocitySlice.y * deltaTime;

		//Get new world bounds
		ion::Vector2 boundsTopLeft;
		ion::Vector2 boundsBottomRight;
		GetWorldBounds(boundsTopLeft, boundsBottomRight);

		//Find wall
		ion::Vector2i wallProbe((int)(m_worldPos.x + (m_size.x / 2.0f)), (int)(m_worldPos.y + m_floorProbeOffset.y));

		float wallPos = (float)m_world.FindWall(wallProbe, velocitySlice.x > 0.0f ? 1 : -1, (m_size.x / 2.0f) + Constants::World::wallSearchDist);

		if(wallPos >= 0.0f && velocitySlice.x > 0.0f && wallPos < (boundsBottomRight.x))
		{
			//Collision with wall to right, adjust position
			m_worldPos.x = wallPos - m_boundsTopLeft.x - m_boundsSize.x;

			//Kill X velocity/acceleration
			m_velocity.x = 0.0f;
			m_acceleration.x = 0.0f;
			velocitySlice.x = 0.0f;

			//Hit wall
			m_hitWall = true;
		}
		else if(wallPos >= 0.0f && velocitySlice.x < 0.0f && wallPos > boundsTopLeft.x)
		{
			//Collision with wall to left, adjust position
			m_worldPos.x = wallPos - m_boundsTopLeft.x;

			//Kill X velocity/acceleration
			m_velocity.x = 0.0f;
			m_acceleration.x = 0.0f;
			velocitySlice.x = 0.0f;

			//Hit wall
			m_hitWall = true;
		}
		
		if(m_velocity.y < 0.0f)
		{
			//Find floor
			ion::Vector2 floorProbe(m_worldPos.x + m_floorProbeOffset.x, m_worldPos.y + m_floorProbeOffset.y);
			const float objectBottom = m_worldPos.y + m_size.y;
			
			u16 floorFlags = 0;
			float floorHeight = (float)m_world.FindFloor(ion::Vector2i((int)floorProbe.x, (int)floorProbe.y), Constants::World::floorSearchDist, floorFlags);

			//Ignore holes if congiured
			if (!m_ignoreHoles || !(floorFlags & eCollisionTileFlagHole))
			{
				if (floorHeight >= 0.0f && floorHeight <= objectBottom + m_stepHeight)
				{
					//Within step height of floor
					m_closeToFloor = true;
				}

				if ((floorHeight >= 0.0f && floorHeight <= objectBottom) || (m_closeToFloor && m_snapToFloor))
				{
					//Collision with floor, adjust position
					m_worldPos.y = floorHeight - m_size.y;

					//Kill Y velocity/acceleration
					m_velocity.y = 0.0f;
					m_acceleration.y = 0.0f;
					velocitySlice.y = 0.0f;

					//On floor
					m_onFloor = true;
					m_closeToFloor = true;
				}
			}
		}
	}

	return SpriteObj::Update(deltaTime);
}

void PhysicsObj::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	return SpriteObj::Render(renderer, cameraInv, mapSize);
}

void PhysicsObj::AddImpulse(const ion::Vector2& impulse)
{
	m_impulse += impulse;
}