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
	std::string tilesetName;
	std::string stampsName;
	std::string palettesName;
	std::string collisionTilesName;
	std::string collisionMapName;
	std::string gameObjectsName;
	std::string tileMapFgName;
	std::string tileMapBgName;
	LevelFactory levelfactory;
};

class Level
{
public:
	enum class State
	{
		Running,
		ActEnded,
		ChapterEnded
	};

	Level();
	virtual ~Level() {}

	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void End() = 0;

	State GetState() const { return m_state; }
	bool IsRunning() const { return m_state == State::Running; }

protected:
	State m_state;
};
