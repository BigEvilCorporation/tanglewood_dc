///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Boulder.h
// Date:		29th September 2018
// Authors:		Matt Phillips
// Description:	Rolling boulder
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/PhysicsObj.h"
#include "framework/Platform.h"

class Boulder : public PhysicsObj
{
public:
	Boulder(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Boulder();

	//Update
	virtual void Update(float deltaTime);

private:
	void Smash();
	void Respawn();
	void CheckSquashDjakk();

	Platform m_platform;
	bool m_smashed;
	ion::Vector2 m_spawnPos;
};
