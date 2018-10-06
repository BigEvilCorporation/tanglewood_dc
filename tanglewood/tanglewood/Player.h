///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Player.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Base for player characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Colour.h"

#include "framework/Character.h"
#include "framework/State.h"

class Player : public Character
{
public:
	enum class InteractionType
	{
		None,
		Push
	};

	Player(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
    virtual ~Player();
    
    //Update
    virtual void Update(float deltaTime);

	void BeginInteract();
	void EndInteract();

	void BeginAbility();
	void EndAbility();

	void SwitchColour(ColourAbility colour);

	ColourAbility m_colour;

private:

	class Ability : public State
	{
	public:
		virtual void BeginUse() = 0;
		virtual void EndUse() = 0;
	};

	class AbilityGlide : public Ability
	{
	public:
		AbilityGlide(Player& player)
			: m_player(player)
		{
			m_active = false;
		}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);
		virtual void OnExitState(State* newState);

		virtual void BeginUse();
		virtual void EndUse();

		Player& m_player;
		bool m_active;
	};

	class AbilityTimeSlow : public Ability
	{
	public:
		AbilityTimeSlow(Player& player)
			: m_player(player)
		{
			m_active = false;
		}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);
		virtual void OnExitState(State* newState);

		virtual void BeginUse();
		virtual void EndUse();

		Player& m_player;
		bool m_active;
		float m_speedScale;
	};

	bool TryInteractPushable();
	bool TryInteractFuzzl();

	void UpdatePushable();

	InteractionType m_activeInteraction;

	StateMachine m_abilityState;
	Ability* m_activeAbility;
	float m_abilityTimer;

	PhysicsObj* m_currentPushable;
};
