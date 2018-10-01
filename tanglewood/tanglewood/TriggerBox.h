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
	TriggerBox(World& world, const GameObject& gameObject, const GameObjectType& gameObjType);
	virtual ~TriggerBox();

	//Update/render
	virtual void Update(float deltaTime);

	static void RegisterPotentialOccupant(const Entity& entity);
	static void UnregisterPotentialOccupant(const Entity& entity);

	static void RegisterTriggerFunc(const std::string& name, std::function<void(const TriggerBox&)> const& onTriggered);
	static void UnregisterTriggerFunc(const std::string& name);

private:
	void ReadVars(const std::vector<GameObjectVariable>& vars);

	static std::vector<const Entity*> s_triggerEntities;
	static std::map<std::string, std::function<void(const TriggerBox&)>> s_triggerFuncMap;
	std::function<void(const TriggerBox&)> m_onTriggered;
};