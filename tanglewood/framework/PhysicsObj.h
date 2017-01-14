///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PhysicsObj.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Physics object and collision handling
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "SpriteObj.h"

class PhysicsObj : public SpriteObj
{
public:
	PhysicsObj(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType);
	virtual ~PhysicsObj();

	//Update/render
	virtual void Update(float deltaTime);
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);

	ion::Vector2 m_velocity;
	ion::Vector2 m_maxVelocity;
	ion::Vector2 m_acceleration;
	ion::Vector2 m_deceleration;
	ion::Vector2 m_floorProbeOffset;

	static const float s_floorSearchDist;
};