///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		TriggerBox.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Trigger box
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "TriggerBox.h"

#include <ion/core/string/String.h>
#include <ion/core/utils/STL.h>

std::vector<const Entity*> TriggerBox::s_triggerEntities;
std::map<std::string, std::function<void(const TriggerBox&)>> TriggerBox::s_triggerFuncMap;

TriggerBox::TriggerBox(World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
	: Entity(world, gameObject, gameObjType, nullptr)
{
	m_triggerCount = 0;
	m_triggerOnce = false;

	ReadVars(gameObject.GetVariables());
}

TriggerBox::~TriggerBox()
{

}

void TriggerBox::Update(float deltaTime)
{
	if (m_triggerCount == 0 || !m_triggerOnce)
	{
		if (m_onTriggered)
		{
			for (int i = 0; i < s_triggerEntities.size(); i++)
			{
				if (Intersects(*s_triggerEntities[i]))
				{
					m_onTriggered(*this);
					m_triggerCount++;
				}
			}
		}
	}
}

void TriggerBox::ReadVars(const std::vector<GameObjectVariable>& vars)
{
	for (int i = 0; i < vars.size(); i++)
	{
		if (ion::string::CompareNoCase(vars[i].m_name, "TriggerBox_Routine"))
		{
			std::map<std::string, std::function<void(const TriggerBox&)>>::iterator it = s_triggerFuncMap.find(vars[i].m_value);

			if (it == s_triggerFuncMap.end())
			{
				//ion::debug::error << "TriggerBox::ReadVars() - Trigger function not found: " << vars[i].m_value << ion::debug::end;
			}
			else
			{
				m_onTriggered = it->second;
			}
		}
		else if (ion::string::CompareNoCase(vars[i].m_name, "TriggerBox_TriggerOnce"))
		{
			if (vars[i].m_value == "0x1")
			{
				m_triggerOnce = true;
			}
		}
	}
}

void TriggerBox::RegisterPotentialOccupant(const Entity& entity)
{
	s_triggerEntities.push_back(&entity);
}

void TriggerBox::UnregisterPotentialOccupant(const Entity& entity)
{
	ion::utils::stl::FindAndRemove(s_triggerEntities, &entity);
}

void TriggerBox::RegisterTriggerFunc(const std::string& name, std::function<void(const TriggerBox&)> const& onTriggered)
{
	s_triggerFuncMap.insert(std::make_pair(name, onTriggered));
}

void TriggerBox::UnregisterTriggerFunc(const std::string& name)
{
	s_triggerFuncMap.erase(name);
}