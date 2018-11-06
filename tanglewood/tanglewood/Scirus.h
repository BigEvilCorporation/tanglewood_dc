///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Scirus.h
// Date:		6th November 2018
// Authors:		Matt Phillips
// Description:	Scirus enemy
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Enemy.h"

#include "framework/State.h"

class Scirus : public Enemy
{
public:
	Scirus(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Scirus();

	//Update/render
	virtual void Update(float deltaTime);

private:
	class StateIdle : public State
	{
	public:
		StateIdle(Scirus& scirus)
			: m_scirus(scirus) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Scirus& m_scirus;
	};

	class StateFriendly : public State
	{
	public:
		StateFriendly(Scirus& scirus)
			: m_scirus(scirus) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Scirus& m_scirus;
	};

	class StateHostile : public State
	{
	public:
		StateHostile(Scirus& scirus)
			: m_scirus(scirus) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Scirus& m_scirus;
	};

	StateMachine m_stateMachine;
};