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
#include "Animations.h"

#include <ion/core/utils/STL.h>
#include <ion/maths/Geometry.h>
#include <ion/core/string/String.h>

std::vector<Character*> Mushroom::s_potentialUsers;

Mushroom::OrientationData Mushroom::s_orientationData[(int)Orientation::Count] =
{
	{ "BouncePlantOrient_0",	"mushroom_180",	false, true,	ion::Vector2(0.0f,								-Constants::Mushroom::impulse),		ion::Vector2(1.0f, 0.0f) }, //Pointing down
	{ "BouncePlantOrient_45",	"mushroom_225",	true, true,		ion::Vector2(-Constants::Mushroom::impulseDiag,	-Constants::Mushroom::impulseDiag),	ion::Vector2(0.0f, 0.0f) },
	{ "BouncePlantOrient_90",	"mushroom_270",	true, false,	ion::Vector2(-Constants::Mushroom::impulse,	0.0f),									ion::Vector2(0.0f, 1.0f) }, //Pointing left
	{ "BouncePlantOrient_135",	"mushroom_225",	true, false,	ion::Vector2(-Constants::Mushroom::impulseDiag,Constants::Mushroom::impulseDiag),	ion::Vector2(0.0f, 0.0f) },
	{ "BouncePlantOrient_180",	"mushroom_180",	false, false,	ion::Vector2(0.0f,								Constants::Mushroom::impulse),		ion::Vector2(1.0f, 0.0f) }, //Pointing up
	{ "BouncePlantOrient_225",	"mushroom_225",	false, false,	ion::Vector2(Constants::Mushroom::impulseDiag,	Constants::Mushroom::impulseDiag),	ion::Vector2(0.0f, 0.0f) },
	{ "BouncePlantOrient_270",	"mushroom_270",	false, true,	ion::Vector2(Constants::Mushroom::impulse,		0.0f),								ion::Vector2(0.0f, 1.0f) }, //Pointing right
	{ "BouncePlantOrient_315",	"mushroom_225",	false, true,	ion::Vector2(Constants::Mushroom::impulseDiag,	-Constants::Mushroom::impulseDiag),	ion::Vector2(0.0f, 0.0f) },
};

Mushroom::Mushroom(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: SpriteObj(world, gameObject, gameObjType, actor)
{
	m_orientation = Orientation::Orient180;

	m_animIdle = Animations::Mushroom::Orient180::idle;
	m_animBounce = Animations::Mushroom::Orient180::bounce;

	ReadVars(gameObject.GetVariables());

	//Set up for orientation
	m_orientationData = s_orientationData[(int)m_orientation];

	//Get animation variant
	m_animIdle.sheetName = m_orientationData.spriteSheet;
	m_animBounce.sheetName = m_orientationData.spriteSheet;

	//Sprite flip
	m_flippedX = m_orientationData.flipX;
	m_flippedY = m_orientationData.flipY;

	//Play idle anim
	PlayAnimation(m_animIdle);
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
				//Mask current velocity
				character.m_velocity *= m_orientationData.reflectionMask;

				//Bounce!
				character.m_velocity += m_orientationData.impulse;

				//Set jumping
				character.m_jumping = true;

				//Set bounce animation, queue idle
				PlayAnimation(m_animBounce);
				QueueAnimation(m_animIdle);
			}
		}
	}
}

void Mushroom::ReadVars(const std::vector<GameObjectVariable>& vars)
{
	for (int i = 0; i < vars.size(); i++)
	{
		if (ion::string::CompareNoCase(vars[i].m_name, "BouncePlant_Orientation"))
		{
			bool orientFound = false;

			for (int j = 0; j < (int)Orientation::Count && !orientFound; j++)
			{
				if (ion::string::CompareNoCase(vars[i].m_value, s_orientationData[j].varName))
				{
					m_orientation = (Orientation)j;
					orientFound = true;
				}
			}

			ion::debug::Assert(orientFound, "Mushroom::ReadVars() - Invalid BouncePlant_Orientation");
		}
	}
}

void Mushroom::RegisterPotentialUser(Character& user)
{
	s_potentialUsers.push_back(&user);
}

void Mushroom::UnregisterPotentialUser(Character& user)
{
	ion::utils::stl::FindAndRemove(s_potentialUsers, &user);
}