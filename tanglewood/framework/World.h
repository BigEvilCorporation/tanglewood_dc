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
#include <ion/core/utils/STL.h>

#include "Stamp.h"
#include "Plane.h"

//TODO: Doesn't belong in framework
#include "tanglewood/PlayerController.h"

#include <vector>
#include <map>

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

	//Entity map
	template <typename T> void AddEntity(T& entity);
	template <typename T> void RemoveEntity(T& entity);
	template <typename T> const std::vector<T*>& GetEntities() const;

	//Find entities
	Entity* FindEntity(const std::string& name);

	//Physics world
	void RegisterPushableObject(PhysicsObj& physicsObj);
	void UnregisterPushableObject(PhysicsObj& physicsObj);
	const std::vector<PhysicsObj*>& GetPushableObjects() const;
	float GetGravity() const { return m_gravity; }

	//Player(s)
	//TODO: Doesn't belong in framework
	PlayerController* GetPlayerController() const { return m_playerController; }

private:
	ion::Vector2 m_mapSizeFg;
	ion::Vector2 m_mapSizeBg;
	ion::Vector2 m_cameraPos;

	//Beehive project files
	Project* m_levelData;
	std::map<ActorId, Actor> m_actors;

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
	std::map<std::string, std::vector<Entity*>> m_entitiesByType;
	std::vector<PhysicsObj*> m_pushableObjs;
    
    //Player controller
	//TODO: Doesn't belong in framework
    PlayerController* m_playerController;
};

template <typename T> void World::AddEntity(T& entity)
{
	std::map<std::string, std::vector<Entity*>>::iterator it = m_entitiesByType.find(typeid(T).name());

	if (it == m_entitiesByType.end())
	{
		it = m_entitiesByType.insert(std::make_pair(typeid(T).name(), std::vector<Entity*>())).first;
	}

	it->second.push_back(&entity);
}

template <typename T> void World::RemoveEntity(T& entity)
{
	std::map<std::string, std::vector<Entity*>>::iterator it = m_entitiesByType.find(typeid(T).name());
	ion::debug::Assert(it != m_entitiesByType.end(), "World::GetEntities<T>() - Invalid type");

	ion::utils::stl::FindAndRemove(it->second, (Entity*)&entity);

	if (it->second.empty())
	{
		m_entitiesByType.erase(it);
	}
}

template <typename T> const std::vector<T*>& World::GetEntities() const
{
	std::map<std::string, std::vector<Entity*>>::const_iterator it = m_entitiesByType.find(typeid(T).name());
	ion::debug::Assert(it != m_entitiesByType.end(), "World::GetEntities<T>() - Invalid type");
	return (std::vector<T*>&)it->second;
}