///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Djakk.h
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Djakk enemy
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Enemy.h"

#include "framework/State.h"

class Djakk : public Enemy
{
public:
	Djakk(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Djakk();

	//Update/render
	virtual void Update(float deltaTime);

	void BeginChase(bool roar);

	void BeginTame();
	void EndTame();

	void BeginRide(Character& jockey);
	void EndRide(bool buck);

private:
	class StateIdle : public State
	{
	public:
		StateIdle(Djakk& djakk)
			: m_djakk(djakk) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Djakk& m_djakk;
	};

	class StateChase : public State
	{
	public:
		StateChase(Djakk& djakk)
			: m_djakk(djakk) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Djakk& m_djakk;
	};

	class StateSearch : public State
	{
	public:
		StateSearch(Djakk& djakk)
			: m_djakk(djakk) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		float m_waitTimer;
		float m_targetPosX;

		Djakk& m_djakk;
	};

	class StateBite : public State
	{
	public:
		StateBite(Djakk& djakk)
			: m_djakk(djakk) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Djakk& m_djakk;
	};

	class StateTamed : public State
	{
	public:
		StateTamed(Djakk& djakk)
			: m_djakk(djakk) {}

		virtual void OnEnterState();
		virtual void OnExitState();
		virtual void OnUpdateState(float deltaTime);

		Djakk& m_djakk;
	};

	class StateBucking : public State
	{
	public:
		StateBucking(Djakk& djakk)
			: m_djakk(djakk) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);

		Djakk& m_djakk;
	};

	StateMachine m_stateMachine;
	Character* m_jockey;
};
