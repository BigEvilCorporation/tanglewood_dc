///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Hogg.h
// Date:		4th October 2018
// Authors:		Matt Phillips
// Description:	Hogg enemy
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Enemy.h"

#include "framework/State.h"

class Hogg : public Enemy
{
public:
	Hogg(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Hogg();

	//Update/render
	virtual void Update(float deltaTime);

private:
	class StateIdle : public State
	{
	public:
		StateIdle(Hogg& hogg)
			: m_hogg(hogg) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Hogg& m_hogg;
	};

	class StateChase : public State
	{
	public:
		StateChase(Hogg& hogg)
			: m_hogg(hogg) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Hogg& m_hogg;
	};

	class StateStuck : public State
	{
	public:
		StateStuck(Hogg& hogg)
			: m_hogg(hogg) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		float m_stuckTimer;

		Hogg& m_hogg;
	};

	StateMachine m_stateMachine;
};