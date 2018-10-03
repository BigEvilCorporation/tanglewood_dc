///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Flue.cpp
// Date:		25th September 2018
// Authors:		Matt Phillips
// Description:	Flue hole
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Flue.h"
#include "Constants.h"
#include "Globals.h"
#include "framework/World.h"

#include <ion/core/string/String.h>
#include <ion/core/utils/STL.h>
#include <ion/maths/Geometry.h>

std::vector<PhysicsObj*> Flue::s_potentialOccupants;

Flue::Flue(World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
	: Entity(world, gameObject, gameObjType, nullptr)
{
	m_world.AddEntity<Flue>(*this);

	ReadVars(gameObject.GetVariables());

	m_ejectTime = Constants::Flue::defaultEjectTime;
	m_ejectForce = Constants::Flue::defaultEjectForce;
}

Flue::~Flue()
{
	m_world.RemoveEntity<Flue>(*this);
}

void Flue::Update(float deltaTime)
{
	Entity::Update(deltaTime);

	//Update existing occupants
	for (int i = 0; i < m_occupants.size(); i++)
	{
		Occupant& occupant = m_occupants[i];

		occupant.occupiedTime += deltaTime;
		if (occupant.occupiedTime >= m_ejectTime)
		{
			EjectOccupant(*occupant.object);
			ion::utils::stl::FindAndRemove(m_occupants, occupant);
			i++;
		}
	}

	//Check for potential new occupants
	for (int i = 0; i < s_potentialOccupants.size(); i++)
	{
		if (CanHold(*s_potentialOccupants[i]))
		{
			AddOccupant(*s_potentialOccupants[i]);
		}
	}
}

void Flue::ReadVars(const std::vector<GameObjectVariable>& vars)
{
	for (int i = 0; i < vars.size(); i++)
	{
		if (ion::string::CompareNoCase(vars[i].m_name, "Flue_Link"))
		{
			m_linkedFlue = vars[i].m_value;
		}
	}
}

bool Flue::CanHold(PhysicsObj& object) const
{
	//If heading downwards
	if (object.m_velocity.y < 0.0f)
	{
		//If contained by flue
		if (Contains(object))
		{
			//If not already occupying
			if (!ion::utils::stl::Find(m_occupants, Occupant(object)))
			{
				return true;
			}
		}
	}

	return false;
}

void Flue::AddOccupant(PhysicsObj& object)
{
	//Add to occupant list
	m_occupants.push_back(Occupant(object));

	//Stop updating/rendering
	object.m_active = false;
	object.m_visible = false;

	//Clear velocity/acceleration
	object.m_velocity.x = 0.0f;
	object.m_velocity.y = 0.0f;
	object.m_acceleration.x = 0.0f;
	object.m_acceleration.y = 0.0f;

	//Find output flue
	m_outputFlue = this;
	if (!m_linkedFlue.empty())
	{
		m_outputFlue = m_world.FindEntity<Flue>(m_linkedFlue);
		ion::debug::Assert(m_outputFlue, "Flue::AddOccupant() - Could not find output flue");
	}

	//Centre object in output flue
	object.m_worldPos.x = (m_outputFlue->m_worldPos.x + (m_outputFlue->m_size.x / 2.0f)) - (object.m_size.x / 2.0f);
	object.m_worldPos.y = (m_outputFlue->m_worldPos.y + (m_outputFlue->m_size.y / 2.0f)) - (object.m_size.y / 2.0f);
}

void Flue::EjectOccupant(PhysicsObj& object)
{
	//Remove from occupant list
	ion::utils::stl::FindAndRemove(m_occupants, Occupant(object));

	//Allow update/render
	object.m_active = true;
	object.m_visible = true;

	//Fling
	object.AddImpulse(ion::Vector2(0.0f, m_ejectForce));
}

void Flue::RegisterPotentialOccupant(PhysicsObj& occupant)
{
	s_potentialOccupants.push_back(&occupant);
}

void Flue::UnregisterPotentialOccupant(PhysicsObj& occupant)
{
	ion::utils::stl::FindAndRemove(s_potentialOccupants, &occupant);
}