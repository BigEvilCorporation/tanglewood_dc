///////////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		World.h
// Date:		14th January 2017
// Authors:		Matt Phillips
// Description:	Encapsulates and processes the world environment
///////////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Camera.h>
#include <ion/renderer/Window.h>
#include <ion/beehive/Project.h>
#include <ion/beehive/Map.h>
#include <ion/input/Keyboard.h>
#include <ion/input/Gamepad.h>

#include "Stamp.h"
#include "Plane.h"
#include "PlayerController.h"

class World
{
public:
	World();
	~World();

	//Load/setup sprites/level/act
	bool LoadSprites(const std::string& name);
	bool LoadLevel(const std::string& name);
	bool LoadAct(const std::string& levelMap, const std::string& bgMap);
	bool CreateGameObjects();

	//Update/render
	void Update(float deltaTime, ion::render::Camera& camera, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad, const ion::render::Window& window, const ion::Vector2i& screenSize);
	void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv);

	//Set camera position, correcting for viewport size
	void SetCameraPosition(const ion::Vector2& position, ion::render::Camera& camera, const ion::render::Window& window, const ion::Vector2i& screenSize);

	//Perform terrain test
	int FindFloor(const ion::Vector2i& position, int maxSearchLength, u16& tileFlags) const;

	//Perform wall test
	int FindWall(const ion::Vector2i& position, int direction, int maxSearchLength) const;

	float GetGravity() const { return m_gravity; }

private:
	ion::Vector2 m_mapSizeFg;
	ion::Vector2 m_mapSizeBg;
	ion::Vector2 m_cameraPos;

	//Beehive project files
	Project* m_levelData;
	Project* m_spriteData;

	//Beehive maps
	Map* m_currentMap;
	Map* m_backgroundMap;

	//Physics data
	TerrainTileset* m_terrainTileset;
	CollisionMap* m_collisionMap;

	//Current stamp set
	StampSet* m_stampSet;

	//Current planes
	Plane* m_planeFg;
	Plane* m_planeBg;

	//Background colour
	ion::Colour m_bgColour;

	//Gravity
	float m_gravity;
    
    //Entities
    std::vector<Entity*> m_entities;
    
    //Player controller
    PlayerController* m_playerController;
};
