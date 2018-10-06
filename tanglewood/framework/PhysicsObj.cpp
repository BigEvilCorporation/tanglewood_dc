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

PhysicsObj::PhysicsObj(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	world.GetPhysicsWorld().AddObject(*this);

	m_floorProbeOffset = ion::Vector2(m_size.x / 2.0f, m_size.y - Constants::MegaDrive::tileHeight);

	m_maxVelocityX = 100.0f;
	m_maxVelocityXAir = 100.0f;
	m_maxVelocityYUp = 800.0f;
	m_maxVelocityYDown = 800.0f;
	m_deceleration.x = 100.0f;
	m_stepHeight = 1.0f;
	m_minWallHeight = 0.0f;
	m_lastFloorVelocity = 0.0f;

	m_snapToFloor = false;
	m_ignoreHoles = false;
	m_onFloor = false;
	m_closeToFloor = false;
	m_hitWall = false;
}

PhysicsObj::~PhysicsObj()
{
	m_world.GetPhysicsWorld().RemoveObject(*this);
}

void PhysicsObj::PhysicsStep(float deltaTime, const PhysicsWorld& physicsWorld)
{
	if (m_active)
	{
		//Check if velocity is to be cancelled by animation
		if (const AnimType* animType = GetCurrentAnimType())
		{
			if (animType->flags & AnimFlags::FreezeMovementX)
			{
				m_acceleration.x = 0.0f;
				m_velocity.x = 0.0f;
			}

			if (animType->flags & AnimFlags::FreezeMovementY)
			{
				m_acceleration.y = 0.0f;
				m_velocity.y = 0.0f;
			}
		}

		//Apply acceleration
		m_velocity += m_acceleration * deltaTime;

		//Apply deceleration (if on floor and controls idle)
		if (m_onFloor && ion::maths::IsZero(m_acceleration.x))
		{
			if (ion::maths::Abs(m_acceleration.x) <= ion::maths::FLOAT_EPSILON)
			{
				//Clamp
				if (m_velocity.x > 0.0f)
				{
					m_velocity.x -= m_deceleration.x * deltaTime;

					if (m_velocity.x < 0.0f)
					{
						m_velocity.x = 0.0f;
					}
				}
				else if (m_velocity.x < 0.0f)
				{
					m_velocity.x += m_deceleration.x * deltaTime;

					if (m_velocity.x > 0.0f)
					{
						m_velocity.x = 0.0f;
					}
				}
			}
		}

		//Clear hit velocity
		m_lastFloorVelocity = 0.0f;
		m_lastWallVelocity = 0.0f;

		//Apply gravity
		m_velocity.y -= physicsWorld.GetGravity() * deltaTime;

		//Apply impulse
		m_velocity += m_impulse;
		m_impulse.x = 0.0f;
		m_impulse.y = 0.0f;

		//Clamp to max velocity
		float maxVelocityX = m_closeToFloor ? m_maxVelocityX : m_maxVelocityXAir;
		m_velocity.x = ion::maths::Clamp(m_velocity.x, -maxVelocityX, maxVelocityX);
		m_velocity.y = ion::maths::Clamp(m_velocity.y, -m_maxVelocityYDown, m_maxVelocityYUp);

		//If velocity > tile size, time slice it
		int numTimeSteps = 1;
		ion::Vector2 velocitySlice = m_velocity;

		float velocitySize = m_velocity.GetLength() * deltaTime;
		if (velocitySize > Constants::MegaDrive::tileWidth)
		{
			numTimeSteps = ion::maths::Ceil(velocitySize / Constants::MegaDrive::tileWidth);
			velocitySlice = m_velocity / (float)numTimeSteps;
		}

		//Clear floor/wall flags
		m_onFloor = false;
		m_closeToFloor = false;
		m_hitWall = false;

		for (int i = 0; i < numTimeSteps; i++)
		{
			//Apply velocity
			m_worldPos.x += velocitySlice.x * deltaTime;
			m_worldPos.y -= velocitySlice.y * deltaTime;

			//Get new world bounds
			ion::Vector2 boundsTopLeft;
			ion::Vector2 boundsBottomRight;
			GetWorldBounds(boundsTopLeft, boundsBottomRight);
			
			//Get probe pos
			ion::Vector2i wallProbe((int)(m_worldPos.x + (m_size.x / 2.0f)), (int)(m_worldPos.y + m_floorProbeOffset.y));
			float wallPos = 0.0f;

			int direction = velocitySlice.x >= 0.0f ? 1 : -1;
			int searchDistance = (int)(m_size.x / 2.0f) + Constants::World::wallSearchDist;

			//Find barrier first
			wallPos = (float)physicsWorld.FindBarrier(wallProbe, direction, searchDistance, (int)m_minWallHeight);

			//Find wall
			if (wallPos < 0)
			{
				wallPos = (float)physicsWorld.FindWall(wallProbe, direction, searchDistance);
			}

			if (wallPos >= 0.0f && velocitySlice.x > 0.0f && wallPos < (boundsBottomRight.x))
			{
				//Collision with wall to right, adjust position
				m_worldPos.x = wallPos - m_boundsTopLeft.x - m_boundsSize.x;

				//Record hit velocity
				m_lastWallVelocity = m_velocity.x;

				//Kill X velocity/acceleration
				m_velocity.x = 0.0f;
				m_acceleration.x = 0.0f;
				velocitySlice.x = 0.0f;

				//Hit wall
				m_hitWall = true;
			}
			else if (wallPos >= 0.0f && velocitySlice.x < 0.0f && wallPos > boundsTopLeft.x)
			{
				//Collision with wall to left, adjust position
				m_worldPos.x = wallPos - m_boundsTopLeft.x;

				//Record hit velocity
				m_lastWallVelocity = m_velocity.x;

				//Kill X velocity/acceleration
				m_velocity.x = 0.0f;
				m_acceleration.x = 0.0f;
				velocitySlice.x = 0.0f;

				//Hit wall
				m_hitWall = true;
			}

			if (m_velocity.y < 0.0f)
			{
				//Find terrain
				ion::Vector2 floorProbe(m_worldPos.x + m_floorProbeOffset.x, m_worldPos.y + m_floorProbeOffset.y);
				const float objectBottom = m_worldPos.y + m_size.y;

				u16 floorFlags = 0;
				float floorHeight = 0.0f;
				
				//Find platform first
				floorHeight = (float)physicsWorld.FindPlatform(ion::Vector2i((int)floorProbe.x, (int)floorProbe.y), Constants::World::floorSearchDist);

				if (floorHeight < 0.0f)
				{
					//No platform found, try terrain
					floorHeight = (float)physicsWorld.FindFloor(ion::Vector2i((int)floorProbe.x, (int)floorProbe.y), Constants::World::floorSearchDist, floorFlags);
				}

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

						//Record hit velocity
						m_lastFloorVelocity = m_velocity.y;

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
	}
}

void PhysicsObj::AddImpulse(const ion::Vector2& impulse)
{
	m_impulse += impulse;
}