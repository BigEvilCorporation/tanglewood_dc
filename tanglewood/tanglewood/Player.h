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
#include "Djakk.h"

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

	virtual void Move(float speed);
	virtual void Jump();
	virtual void CancelJump();

	void BeginInteract(bool debounce);
	void EndInteract();

	void BeginAbility(bool debounce);
	void EndAbility(bool timedOut);

	void SwitchColour(ColourAbility colour);

	ColourAbility m_colour;

private:
	void StartPaletteLerp(const Palette& source, const Palette& dest, float speed);
	void UpdatePaletteLerp(float deltaTime);

	class Ability : public State
	{
	public:
		virtual void BeginUse(bool debounce) = 0;
		virtual void EndUse(bool timedOut) = 0;
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

		virtual void BeginUse(bool debounce);
		virtual void EndUse(bool timedOut);

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

		virtual void BeginUse(bool debounce);
		virtual void EndUse(bool timedOut);

		Player& m_player;
		bool m_active;
		float m_speedScale;
		Palette m_originalPalette;
	};

	class AbilityBeastTame : public Ability
	{
	public:
		AbilityBeastTame(Player& player)
			: m_player(player)
		{
			m_active = false;
			m_beast = nullptr;
		}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);
		virtual void OnExitState(State* newState);

		virtual void BeginUse(bool debounce);
		virtual void EndUse(bool timedOut);

		void StopRiding(bool buck);

		Player& m_player;
		bool m_active;
		bool m_mounting;
		Djakk* m_beast;
	};

	static const Palette* s_colourPalettes[(int)ColourAbility::Count];

	bool TryInteractPushable();
	bool TryInteractFuzzl();

	void UpdatePushable();

	InteractionType m_activeInteraction;

	StateMachine m_abilityState;
	Ability* m_activeAbility;
	float m_abilityTimer;

	PhysicsObj* m_currentPushable;
	Character* m_currentMount;
	ion::Vector2 m_mountSaddlePos;

	Palette m_sourcePalette;
	Palette m_destPalette;
	float m_paletteLerpTimer;
	float m_paletteLerpSpeed;
};
