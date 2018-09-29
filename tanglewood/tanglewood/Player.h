///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Player.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Base for player characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "Colour.h"

#include "framework/Character.h"
#include "framework/State.h"

class Player : public Character
{
public:
	enum class InteractionType
	{
		None,
		Push
	};

	Player(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
    virtual ~Player();
    
    //Update/render
    virtual void Update(float deltaTime);
    virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);

	void BeginInteract();
	void EndInteract();

	void SwitchColour(ColourAbility colour);

	ColourAbility m_colour;

private:

	class AbilityGlide : public State
	{
	public:
		AbilityGlide(Player& player)
			: m_player(player) {}

		virtual void OnEnterState();
		virtual void OnUpdateState(float deltaTime);
		virtual void OnExitState(State* newState);

		Player& m_player;
	};

	bool TryInteractPushable();
	bool TryInteractFindFuzzl();

	void UpdatePushable();

	InteractionType m_activeInteraction;

	StateMachine m_abilityState;

	PhysicsObj* m_currentPushable;
};
