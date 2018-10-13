///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		DeathBox.h
// Date:		13th October 2018
// Authors:		Matt Phillips
// Description:	Death box
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Character.h"

#include <vector>

class DeathBox : public Entity
{
public:
	DeathBox(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~DeathBox();

	virtual void Update(float deltaTime);

	static void RegisterPotentialVictim(Character& character);
	static void UnregisterPotentialVictim(Character& character);

private:
	static std::vector<Character*> s_victims;
};