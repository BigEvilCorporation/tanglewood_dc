///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		TriggerBox.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Trigger box
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Entity.h"

#include <functional>
#include <string>
#include <map>
#include <vector>

class TriggerBox : public Entity
{
public:
	TriggerBox(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~TriggerBox();

	//Update/render
	virtual void Update(float deltaTime);

	static void RegisterPotentialOccupant(const Entity& entity);
	static void UnregisterPotentialOccupant(const Entity& entity);

	static void RegisterTriggerFunc(const std::string& name, std::function<void(const TriggerBox&)> const& onTriggered);
	static void UnregisterTriggerFunc(const std::string& name);

private:
	enum class TriggerType
	{
		Intersects,
		Contains
	};

	void ReadVars(const std::vector<GameObjectVariable>& vars);

	bool CheckTrigger(const Entity& entity);

	static std::vector<const Entity*> s_triggerEntities;
	static std::map<std::string, std::function<void(const TriggerBox&)>> s_triggerFuncMap;
	std::function<void(const TriggerBox&)> m_onTriggered;

	int m_triggerCount;
	bool m_triggerOnce;
	TriggerType m_triggerType;
};