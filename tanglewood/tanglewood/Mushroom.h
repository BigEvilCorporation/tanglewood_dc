///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Mushroom.h
// Date:		25th September 2018
// Authors:		Matt Phillips
// Description:	Bounce mushroom
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/SpriteObj.h"
#include "framework/Character.h"

class Mushroom : public SpriteObj
{
public:
	Mushroom(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Mushroom();

	//Update
	virtual void Update(float deltaTime);

	static void RegisterPotentialUser(Character& user);
	static void UnregisterPotentialUser(Character& user);

private:
	static std::vector<Character*> s_potentialUsers;

	ion::Vector2 m_bounceVel;
};