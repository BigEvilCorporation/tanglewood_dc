///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Flue.cpp
// Date:		27th September 2018
// Authors:		Matt Phillips
// Description:	Fuzzl nest
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Nest.h"

#include <ion/core/utils/STL.h>

std::vector<Nest*> Nest::s_allNests;

Nest::Nest(World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
	: Entity(world, gameObject, gameObjType, nullptr)
{
	s_allNests.push_back(this);
}

Nest::~Nest()
{
	ion::utils::stl::FindAndRemove(s_allNests, this);
}

std::vector<Nest*>& Nest::GetAll()
{
	return s_allNests;
}