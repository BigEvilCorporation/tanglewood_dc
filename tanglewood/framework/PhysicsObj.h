///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Physics object and collision handling
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "SpriteObj.h"
#include "Physics.h"

class PhysicsObj : public SpriteObj
{
public:
	PhysicsObj(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~PhysicsObj();

	//Step
	void PhysicsStep(float deltaTime, const PhysicsWorld& physicsWorld);

	void AddImpulse(const ion::Vector2& impulse);

	bool CheckCollision(int collisionFlags);

	ion::Vector2 m_velocity;
	ion::Vector2 m_impulse;
	ion::Vector2 m_acceleration;
	ion::Vector2 m_deceleration;
	ion::Vector2 m_floorProbeOffset;
	ion::Vector2 m_ceilingProbeOffset;

	float m_maxVelocityX;
	float m_maxVelocityXAir;
	float m_maxVelocityYUp;
	float m_maxVelocityYDown;

	float m_stepHeight;
	float m_minWallHeight;
	bool m_snapToFloor;

	bool m_physicsEnabled;

	bool m_ignoreHoles;

	//Collision flags
	bool m_onFloor;
	bool m_closeToFloor;
	bool m_hitWall;
	int m_collisionFlags;

	float m_speedScale;

	float m_lastFloorVelocity;
	float m_lastWallVelocity;
};
