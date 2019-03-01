///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Firefly.cpp
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Firefly pickup
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Firefly.h"
#include "Animations.h"
#include "framework/World.h"

Firefly::Firefly(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	world.AddEntity<Firefly>(*this);

	SetColourPalette(Constants::Palettes::palIndexFuzzl);

	//Setup animation
	PlayAnimation(Animations::Firefly::idle);
}

Firefly::~Firefly()
{
	m_world.RemoveEntity<Firefly>(*this);
}

void Firefly::Pickup()
{
	m_active = false;
	m_visible = false;
}