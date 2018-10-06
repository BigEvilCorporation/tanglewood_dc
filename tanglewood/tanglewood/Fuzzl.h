///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Fuzzl.h
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Fuzzl
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Nest.h"
#include "Colour.h"

#include "framework/Character.h"
#include "framework/State.h"

class Fuzzl : public Character
{
public:
	Fuzzl(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Fuzzl();

	//Update
	virtual void Update(float deltaTime);

	bool IsInNest() const;

	ColourAbility m_colour;

	Nest* m_nest;

private:
	class StateIdle : public State
	{
	public:
		StateIdle(Fuzzl& fuzzl)
			: m_fuzzl(fuzzl) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Fuzzl& m_fuzzl;
	};

	class StateWatching : public State
	{
	public:
		StateWatching(Fuzzl& fuzzl)
			: m_fuzzl(fuzzl) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Fuzzl& m_fuzzl;
	};

	class StateRolling : public State
	{
	public:
		StateRolling(Fuzzl& fuzzl)
			: m_fuzzl(fuzzl) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Fuzzl& m_fuzzl;
	};

	class StateNest : public State
	{
	public:
		StateNest(Fuzzl& fuzzl)
			: m_fuzzl(fuzzl) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Fuzzl& m_fuzzl;

		float m_bounceTimer;
	};

	static std::string s_spritePrefixes[(int)ColourAbility::Count];

	void ReadVars(const std::vector<GameObjectVariable>& vars);
	Nest* FindNest() const;

	AnimType m_animIdle;
	AnimType m_animWatch;
	AnimType m_animRoll;
	AnimType m_animSleep;

	StateMachine m_stateMachine;
};
