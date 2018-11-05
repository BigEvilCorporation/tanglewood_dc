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

protected:
	//Take occupant from linked flue
	void TakeOccupant(Character& object, const Flue& originalFlue);

private:
	struct Occupant
	{
		Occupant() {}
		Occupant(Character& occupant)
		{
			object = &occupant;
			occupiedTime = 0.0f;
			cameraTarget = false;
		}

		Character* object;
		float occupiedTime;
		bool cameraTarget;

		bool operator == (const Occupant& rhs) const { return object == rhs.object; }
	};

	void ReadVars(const std::vector<GameObjectVariable>& vars);

	bool CanHold(Character& object) const;
	Occupant* AddOccupant(Character& object);
	void EjectOccupant(Character& object);

	static std::vector<Character*> s_potentialOccupants;
	std::vector<Occupant> m_occupants;

	float m_ejectTime;
	float m_ejectForce;
	std::string m_linkedFlue;
};
