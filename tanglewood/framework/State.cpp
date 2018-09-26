///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		State.cpp
// Date:		26th september 2018
// Authors:		Matt Phillips
// Description:	Micro state machine, for embedded object states and A.I.
///////////////////////////////////////////////////////////////

#include "State.h"

#include <ion/core/debug/Debug.h>

State::State()
{
	m_stateMachine = nullptr;
	m_id = ion::GenerateUUID64();
}

StateMachine::StateMachine()
{
	m_currentState = nullptr;
}

StateMachine::~StateMachine()
{
	for (std::map<std::string, State*>::iterator it = m_states.begin(), end = m_states.end(); it != end; ++it)
	{
		delete it->second;
	}
}

void StateMachine::AddState(State* state, const std::string& name)
{
	state->m_name = name;
	state->m_stateMachine = this;
	m_states.insert(std::make_pair(name, state));
}

void StateMachine::SetState(const std::string& name)
{
	std::map<std::string, State*>::iterator it = m_states.find(name);
	ion::debug::Assert(it != m_states.end(), "StateMachine::SetState() - State not found");
	SetState(it->second);
}

void StateMachine::SetState(State* state)
{
	if (m_currentState)
	{
		m_currentState->OnExitState(state);
	}

	m_currentState = state;

	if (m_currentState)
	{
		m_currentState->OnEnterState();
	}
}

void StateMachine::Update(float deltaTime)
{
	if (m_currentState)
	{
		m_currentState->OnUpdateState(deltaTime);
	}
}
