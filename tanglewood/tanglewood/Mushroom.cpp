///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Mushroom.cpp
// Date:		25th September 2018
// Authors:		Matt Phillips
// Description:	Bounce mushroom
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Mushroom.h"
#include "Constants.h"

#include <ion/core/utils/STL.h>
#include <ion/maths/Geometry.h>

std::vector<Character*> Mushroom::s_potentialUsers;

Mushroom::Mushroom(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	m_bounceVel.x = Constants::Mushroom::defaultBounceVelX;
	m_bounceVel.y = Constants::Mushroom::defaultBounceVelY;
}

Mushroom::~Mushroom()
{

}

void Mushroom::Update(float deltaTime)
{
	SpriteObj::Update(deltaTime);

	//Find touching users
	for (int i = 0; i < s_potentialUsers.size(); i++)
	{
		Character& character = *s_potentialUsers[i];

		//If not heading upwards
		if (character.m_velocity.y <= 0.0f)
		{
			//If touching
			if (Intersects(character))
			{
				//Bounce!
				if (m_bounceVel.x != 0.0f)
				{
					character.m_velocity.x = m_bounceVel.x;
				}

				if (m_bounceVel.y != 0.0f)
				{
					character.m_velocity.y = m_bounceVel.y;
				}

				//Set jumping
				character.m_jumping = true;

				//Set bounce animation, queue idle
				SetAnimation("mushroom_180", "bounce", false);
				QueueAnimation("mushroom_180", "idle", true);
			}
		}
	}
}

void Mushroom::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	SpriteObj::Render(renderer, cameraInv, mapSize);
}

void Mushroom::RegisterPotentialUser(Character& user)
{
	s_potentialUsers.push_back(&user);
}

void Mushroom::UnregisterPotentialUser(Character& user)
{
	ion::utils::stl::FindAndRemove(s_potentialUsers, &user);
}