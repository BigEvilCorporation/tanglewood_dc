///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		DeathBox.cpp
// Date:		13th October 2018
// Authors:		Matt Phillips
// Description:	Death box
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "DeathBox.h"

#include <ion/core/utils/STL.h>

std::vector<Character*> DeathBox::s_victims;

DeathBox::DeathBox(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Entity(world, gameObject, gameObjType, actor)
{

}

DeathBox::~DeathBox()
{

}

void DeathBox::Update(float deltaTime)
{
	for (int i = 0; i < s_victims.size(); i++)
	{
		if ( s_victims[i]->m_alive && Intersects(*s_victims[i]))
		{
			s_victims[i]->Kill();
		}
	}
}

void DeathBox::RegisterPotentialVictim(Character& character)
{
	s_victims.push_back(&character);
}

void DeathBox::UnregisterPotentialVictim(Character& character)
{
	ion::utils::stl::FindAndRemove(s_victims, &character);
}
