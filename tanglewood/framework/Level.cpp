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

Level::Level(const LevelData& levelData)
	: m_levelData(levelData)
{
	m_levelRunning = true;
}

void Level::EndLevel()
{
	m_levelRunning = false;
}
