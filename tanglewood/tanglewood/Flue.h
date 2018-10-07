///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Flue.h
// Date:		25th September 2018
// Authors:		Matt Phillips
// Description:	Flue hole
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Entity.h"
#include "framework/Character.h"

class Flue : public Entity
{
public:
	Flue(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~Flue();

	//Update
	virtual void Update(float deltaTime);

	static void RegisterPotentialOccupant(Character& occupant);
	static void UnregisterPotentialOccupant(Character& occupant);

private:
	void ReadVars(const std::vector<GameObjectVariable>& vars);

	bool CanHold(Character& object) const;
	void AddOccupant(Character& object);
	void EjectOccupant(Character& object);

	struct Occupant
	{
		Occupant() {}
		Occupant(Character& occupant)
		{
			object = &occupant;
			occupiedTime = 0.0f;
		}

		Character* object;
		float occupiedTime;

		bool operator == (const Occupant& rhs) const { return object == rhs.object; }
	};

	static std::vector<Character*> s_potentialOccupants;
	std::vector<Occupant> m_occupants;

	float m_ejectTime;
	float m_ejectForce;
	std::string m_linkedFlue;
	Flue* m_outputFlue;
};
