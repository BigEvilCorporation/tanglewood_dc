///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Stone.cpp
// Date:		6th November 2018
// Authors:		Matt Phillips
// Description:	Checkpoint totem
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Checkpoint.h"
#include "Palettes.h"
#include "Globals.h"

#include "framework/World.h"

#include "tanglewood/Player.h"

Checkpoint::Checkpoint(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	m_world.AddEntity<Checkpoint>(*this);
	m_activated = false;

	//Use shared player palette
#if USE_PALETTE_TEXTURES
	SetPaletteTexture(Assets::Palettes::Player::shared);
#endif

	//Set inactive sprite
	SetSpriteSheet("inactive");
}

Checkpoint::~Checkpoint()
{
	m_world.RemoveEntity<Checkpoint>(*this);
}

void Checkpoint::Update(float deltaTime)
{
	SpriteObj::Update(deltaTime);

	if (!m_activated)
	{
		if (Globals::Players::player1->GetWorldCentre().x >= GetWorldCentre().x)
		{
			//TODO: Game serialisation

			m_activated = true;

			//Set active sprite
			SetSpriteSheet("active");
		}
	}
}