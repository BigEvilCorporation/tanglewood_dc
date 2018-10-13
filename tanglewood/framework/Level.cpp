///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Level.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level base
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Level.h"

Level::Level()
{
	m_state = State::Running;
}
