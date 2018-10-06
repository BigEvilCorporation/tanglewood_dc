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
	enum class Orientation
	{
		Orient0,
		Orient45,
		Orient90,
		Orient135,
		Orient180,
		Orient225,
		Orient270,
		Orient315,

		Count
	};

	Mushroom(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Mushroom();

	//Update
	virtual void Update(float deltaTime);

	static void RegisterPotentialUser(Character& user);
	static void UnregisterPotentialUser(Character& user);

private:
	void ReadVars(const std::vector<GameObjectVariable>& vars);

	struct OrientationData
	{
		std::string varName;
		std::string spriteSheet;
		bool flipX;
		bool flipY;
		ion::Vector2 impulse;
		ion::Vector2 reflectionMask;
	};

	static OrientationData s_orientationData[(int)Orientation::Count];
	static std::vector<Character*> s_potentialUsers;

	Orientation m_orientation;
	OrientationData m_orientationData;

	AnimType m_animIdle;
	AnimType m_animBounce;
};