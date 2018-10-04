///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Character.h
// Date:		15th January 2017
// Authors:		Matt Phillips
// Description:	Base for all characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "PhysicsObj.h"
#include "Animation.h"

class Character : public PhysicsObj
{
public:
	enum class CharacterAnimations
	{
		Idle,
		Dead,
		Run,
		Walk,
		Jump,
		Hang,
		Glide,
		PushLight,
		PushHeavy,
		Fall,
		WalkToRun,
		WaterWade,

		Count
	};

	Character(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Character();

	//Update
	virtual void Update(float deltaTime);

	void Move(float speed);
	void Jump();

	void Kill();

	bool m_allowRunning;
	bool m_running;
	bool m_jumping;
	bool m_pushingLight;
	bool m_pushingHeavy;

	bool m_alive;

	bool m_manualAnimation;

	float m_walktoRunVelocity;
	float m_maxVelocityXWalking;
	float m_maxVelocityXRunning;

protected:
	std::pair<std::string, AnimType> m_characterAnimations[(int)CharacterAnimations::Count];

private:
	void SetCharacterAnimation(CharacterAnimations animation, bool interrupt = true);
	void UpdateAnimation();

	bool m_walkToRunAnimTransition;
};
