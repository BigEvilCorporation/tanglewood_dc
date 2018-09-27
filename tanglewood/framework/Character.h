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
		Push,
		PushHeavy,
		Fall,
		WalkToRun,
		WaterWade,

		Count
	};

	Character(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Character();

	//Update/render
	virtual void Update(float deltaTime);
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);

	void Move(float speed);
	void Jump();

	bool m_allowRunning;
	bool m_running;
	bool m_jumping;

	bool m_manualAnimation;

	float m_walktoRunVelocity;
	float m_maxVelocityXWalking;
	float m_maxVelocityXRunning;

protected:
	std::pair<std::string, std::string> m_characterAnimations[(int)CharacterAnimations::Count];

private:
	void SetCharacterAnimation(CharacterAnimations animation, bool loop = true, bool interrupt = true);
	void UpdateAnimation();

	bool m_walkToRunAnimTransition;
};
