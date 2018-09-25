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

class PhysicsObj : public SpriteObj
{
public:
	PhysicsObj(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~PhysicsObj();

	//Update/render
	virtual void Update(float deltaTime);
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);

	void AddImpulse(const ion::Vector2& impulse);

	ion::Vector2 m_velocity;
	ion::Vector2 m_impulse;
	ion::Vector2 m_acceleration;
	ion::Vector2 m_deceleration;
	ion::Vector2 m_floorProbeOffset;

	float m_maxVelocityX;
	float m_maxVelocityYUp;
	float m_maxVelocityYDown;

	float m_stepHeight;
	bool m_snapToFloor;

	bool m_ignoreHoles;

	bool m_onFloor;
	bool m_closeToFloor;
	bool m_hitWall;
};
