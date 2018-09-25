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
#include "framework/PhysicsObj.h"

class Flue : public Entity
{
public:
	Flue(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType);
	virtual ~Flue();

	//Update
	virtual void Update(float deltaTime);

	static void RegisterPotentialOccupant(PhysicsObj& occupant);
	static void UnregisterPotentialOccupant(PhysicsObj& occupant);

private:
	bool CanHold(PhysicsObj& object) const;
	void AddOccupant(PhysicsObj& object);
	void EjectOccupant(PhysicsObj& object);

	struct Occupant
	{
		Occupant() {}
		Occupant(PhysicsObj& occupant)
		{
			object = &occupant;
			occupiedTime = 0.0f;
		}

		PhysicsObj* object;
		float occupiedTime;

		bool operator == (const Occupant& rhs) const { return object == rhs.object; }
	};

	static std::vector<PhysicsObj*> s_potentialOccupants;
	std::vector<Occupant> m_occupants;

	float m_ejectTime;
	float m_ejectForce;
};
