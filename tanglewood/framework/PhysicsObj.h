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
};