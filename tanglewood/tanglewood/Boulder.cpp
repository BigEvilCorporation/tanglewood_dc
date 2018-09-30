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
#include "Animations.h"

#include "framework/World.h"

Boulder::Boulder(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: PhysicsObj(world, gameObject, gameObjType, actor)
{
	//Setup animation
	PlayAnimation(Animations::Boulder::roll);

	//Add as pushable
	m_world.GetPhysicsWorld().AddPushableObject(*this);

	//Add platform
	m_world.GetPhysicsWorld().AddPlatform(m_platform);
}

Boulder::~Boulder()
{
	m_world.GetPhysicsWorld().RemovePushableObject(*this);
	m_world.GetPhysicsWorld().RemovePlatform(m_platform);
}

void Boulder::Update(float deltaTime)
{
	PhysicsObj::Update(deltaTime);

	//Scale anim speed based on velocity
	float animSpeed = m_velocity.x * Constants::Fuzzl::animSpeedVelocityMul;
	GetCurrentAnimation()->SetPlaybackSpeed(animSpeed);
}

void Boulder::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	PhysicsObj::Render(renderer, cameraInv, mapSize);
}
