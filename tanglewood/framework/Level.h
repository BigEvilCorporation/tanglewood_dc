///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Level.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Level base
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <functional>

class Level;

typedef std::function<Level*()> LevelFactory;

struct LevelDescriptor
{
	std::string spriteDataFile;
	std::string levelDataFile;
	std::string actName;
	std::string bgName;
	LevelFactory levelfactory;
};

class Level
{
public:
	Level();

	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void End() = 0;

	void EndLevel();

	bool IsRunning() const { return m_levelRunning; }

private:
	bool m_levelRunning;
};
