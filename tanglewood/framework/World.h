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
#include <ion/core/string/String.h>

#include "Stamp.h"
#include "Plane.h"
#include "Physics.h"

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
	Project* LoadLevelData(const std::string& name);
	bool LoadAct(Project& project, int levelIdx, const std::string& levelMap, const std::string& bgMap);
	bool LoadGameObjectTypes(Project& project, const std::string& name);
	bool CreateGameObjects();

	//Reset world to default state
	void Reset();
	void DeleteGameObjects();

	int GetLevelIdx() const { return m_levelIdx; }

	//Update/render
	void Update(float deltaTime, ion::render::Camera& camera, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad, const ion::render::Window& window, const ion::Vector2i& screenSize);
	void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, const ion::render::Viewport& viewport, const ion::Matrix4& cameraInv);

	//Set camera position, correcting for viewport size
	void SetCameraPosition(const ion::Vector2& position, ion::render::Camera& camera, const ion::render::Window& window, const ion::Vector2i& screenSize);

	//Entity map
	template <typename T> void AddEntity(T& entity);
	template <typename T> void RemoveEntity(T& entity);
	template <typename T> const std::vector<T*>& GetEntities() const;

	//Find entities
	template <typename T> T* FindEntity(const std::string& name) const;

	//Find sprite actor
	const Actor* FindActor(const std::string& name) const;

	//Physics world
	PhysicsWorld& GetPhysicsWorld() { return *m_physicsWorld; }

	//Player(s)
	//TODO: Doesn't belong in framework
	PlayerController* GetPlayerController() const { return m_playerController; }

	//Effects
	bool BeginFade(float speed);
	bool IsFading() const;

	//Palette lerp
	void SetPalette(const Palette& palette);
	const Palette& GetPalette() const;
	void BeginPaletteLerp(const Palette& dest, float speed);

private:
	void UpdateFader(float deltaTime);
	void UpdatePaletteLerp(float deltaTime);

	int m_levelIdx;

	ion::Vector2 m_mapSizeFg;
	ion::Vector2 m_mapSizeBg;
	ion::Vector2 m_cameraPos;

	//TODO: Move to global assets
	std::map<ActorId, Actor> m_actors;

	TGameObjectTypeMap m_gameObjectTypes;

	//Beehive maps
	//Map m_currentMap;
	Map m_backgroundMap;

	//Current tileset
	Tileset m_tileset;

	//Current stamp set
	TStampMap m_stamps;

	//Current stamp maps
	ion::Vector2i m_mapSizeTilesFg;
	ion::Vector2i m_mapSizeTilesBg;
	TStampPosMap m_stampMapFg;
	TStampPosMap m_stampMapBg;

	//Current palettes
	std::vector<Palette> m_palettes;

	//Curent game objects
	TGameObjectPosMap m_gameObjects;

	//Current stamp set
	StampSet* m_stampSet;

	//Current planes
	Plane* m_planeFg;
	Plane* m_planeBg;

	//Background colour
	ion::Colour m_bgColour;

	//Physics world
	PhysicsWorld* m_physicsWorld;
    
    //Entities
    std::vector<Entity*> m_entities;
	std::map<std::string, std::vector<Entity*>> m_entitiesByType;
    
    //Player controller
	//TODO: Doesn't belong in framework
    PlayerController* m_playerController;

	//Effects
	float m_fader;
	float m_fadeSpeed;
	ion::render::Quad* m_fadeQuad;
	ion::render::Material* m_fadeMaterial;

	//Palette lerp
	Palette m_sourcePalette;
	Palette m_currentPalette;
	float m_paletteLerpTimer;
	float m_paletteLerpSpeed;
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

template <typename T> T* World::FindEntity(const std::string& name) const
{
	T* entity = nullptr;
	const std::vector<T*>& entities = GetEntities<T>();

	typename std::vector<T*>::const_iterator it = std::find_if(entities.begin(), entities.end(), [&name](const T* rhs) { return ion::string::CompareNoCase(name, rhs->m_name); });
	if (it != entities.end())
	{
		entity = *it;
	}

	return entity;
}