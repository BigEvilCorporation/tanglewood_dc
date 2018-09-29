///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		State.h
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Micro state machine, for embedded object states and A.I.
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/core/cryptography/UUID.h>

#include <string>
#include <map>

class State;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void AddState(State* state, const std::string& name);

	void SetState(const std::string& name);
	void SetState(State* state);

	const State* GetCurrentState() const { return m_currentState; }

	void Update(float deltaTime);

private:
	std::map<std::string, State*> m_states;
	State* m_currentState;
};

class State
{
public:
	State();

	const std::string& GetName() const { return m_name; }
	const ion::UUID64 GetId() const { return m_id; }

	virtual void OnEnterState() {}
	virtual void OnUpdateState(float deltaTime) {}
	virtual void OnExitState(State* nextState) {}

protected:
	friend class StateMachine;
	StateMachine* m_stateMachine;
	std::string m_name;
	ion::UUID64 m_id;
};