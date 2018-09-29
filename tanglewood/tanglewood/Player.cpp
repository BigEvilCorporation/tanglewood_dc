///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Player.h
// Date:		27th January 2017
// Authors:		Matt Phillips
// Description:	Base for player characters
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Player.h"
#include "Constants.h"
#include "Animations.h"
#include "framework/World.h"

#include "Flue.h"
#include "Mushroom.h"
#include "Fuzzl.h"

Player::Player(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Character(world, gameObject, gameObjType, actor)
{
	m_world.AddEntity<Player>(*this);

	m_boundsSize.x = Constants::Player::boundsWidth;
	m_boundsSize.y = Constants::Player::boundsHeight;
	m_boundsTopLeft.x = (m_size.x / 2.0f) - (Constants::Player::boundsWidth / 2);
	m_boundsTopLeft.y = (m_size.y / 2.0f) - (Constants::Player::boundsHeight / 2);
	m_boundsBottomRight.x = m_boundsTopLeft.x + Constants::Player::boundsWidth;
	m_boundsBottomRight.y = m_boundsTopLeft.y + Constants::Player::boundsHeight;

	//Initial colour
	m_colour = ColourAbility::Red;

	m_activeInteraction = InteractionType::None;

	m_currentPushable = nullptr;

	Flue::RegisterPotentialOccupant(*this);
	Mushroom::RegisterPotentialUser(*this);

	//Setup animations
	m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", Animations::Player::idle);
	m_characterAnimations[(int)CharacterAnimations::Dead] = std::make_pair("dead", Animations::Player::dead);
	m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", Animations::Player::run);
	m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", Animations::Player::walk);
	m_characterAnimations[(int)CharacterAnimations::Jump] = std::make_pair("jump", Animations::Player::jump);
	m_characterAnimations[(int)CharacterAnimations::Glide] = std::make_pair("glide", Animations::Player::glide);
	m_characterAnimations[(int)CharacterAnimations::PushLight] = std::make_pair("push", Animations::Player::push);
	m_characterAnimations[(int)CharacterAnimations::PushHeavy] = std::make_pair("pushHeavy", Animations::Player::pushHeavy);
	m_characterAnimations[(int)CharacterAnimations::Fall] = std::make_pair("fall", Animations::Player::fall);
	m_characterAnimations[(int)CharacterAnimations::WalkToRun] = std::make_pair("walktorun", Animations::Player::walktorun);
	m_characterAnimations[(int)CharacterAnimations::WaterWade] = std::make_pair("waterWade", Animations::Player::waterWade);

	//Setup ability states
	m_abilityState.AddState(new AbilityGlide(*this), "glide");
}

Player::~Player()
{
	m_world.RemoveEntity<Player>(*this);
	Flue::UnregisterPotentialOccupant(*this);
	Mushroom::UnregisterPotentialUser(*this);
}

void Player::Update(float deltaTime)
{
	Character::Update(deltaTime);

	//If jumping, ignore terrain holes
	m_ignoreHoles = m_jumping;

	switch (m_activeInteraction)
	{
	case InteractionType::Push:
		UpdatePushable();
		break;
	}

	m_abilityState.Update(deltaTime);
}

void Player::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	Character::Render(renderer, cameraInv, mapSize);
}

void Player::BeginInteract()
{
	if (m_activeInteraction == InteractionType::None)
	{
		if (TryInteractPushable())
		{
			m_activeInteraction = InteractionType::Push;
			m_allowRunning = false;

			if (m_currentPushable->m_size.y > Constants::Player::heavyPushObjectHeight)
			{
				m_pushingHeavy = true;
			}
			else
			{
				m_pushingLight = true;
			}

			return;
		}

		if (TryInteractFindFuzzl())
		{
			return;
		}
	}
}

void Player::EndInteract()
{
	m_activeInteraction = InteractionType::None;
	m_allowRunning = true;
	m_currentPushable = nullptr;
	m_pushingLight = false;
	m_pushingHeavy = false;
}

void Player::SwitchColour(ColourAbility colour)
{
	m_colour = colour;
}

bool Player::TryInteractPushable()
{
	const std::vector<PhysicsObj*>& pushableObjs = m_world.GetPushableObjects();

	//Find intersecting pushable obj
	for (int i = 0; i < pushableObjs.size() && !m_currentPushable; i++)
	{
		if (Intersects(*pushableObjs[i]))
		{
			//TODO: Check facing right direction

			m_currentPushable = pushableObjs[i];
		}
	}

	return m_currentPushable != nullptr;
}

bool Player::TryInteractFindFuzzl()
{
	const std::vector<Fuzzl*>& fuzzls = m_world.GetEntities<Fuzzl>();

	for (int i = 0; i < fuzzls.size(); i++)
	{
		if (Intersects(*fuzzls[i]))
		{
			//Touching a Fuzzl
			if (fuzzls[i]->IsInNest() && fuzzls[i]->m_colour != m_colour)
			{
				//Change colour
				SwitchColour(fuzzls[i]->m_colour);

				//Change ability
				switch (fuzzls[i]->m_colour)
				{
				case ColourAbility::Yellow:
					m_abilityState.SetState("glide");
					break;
				default:
					m_abilityState.SetState(nullptr);
				}

				return true;
			}
		}
	}

	return false;
}

void Player::UpdatePushable()
{
	if (m_currentPushable)
	{
		//Check still intersects
		//TODO: Check still facing right direction
		if (!Intersects(*m_currentPushable))
		{
			EndInteract();
		}
		else
		{
			float playerCentre = GetWorldCentre().x;
			float pushableCentre = m_currentPushable->GetWorldCentre().x;

			//If facing right direction
			if ((!m_flippedX && (pushableCentre > playerCentre))
				|| (m_flippedX && (pushableCentre < playerCentre)))
			{
				//Get bounding boxes
				ion::Vector2 playerTopLeft;
				ion::Vector2 playerBottomRight;
				ion::Vector2 pushableTopLeft;
				ion::Vector2 pushableBottomRight;

				GetWorldBounds(playerTopLeft, playerBottomRight);
				m_currentPushable->GetWorldBounds(pushableTopLeft, pushableBottomRight);

				//Snap to edge
				if (m_flippedX && (pushableBottomRight.x > playerTopLeft.x))
				{
					pushableBottomRight.x = playerTopLeft.x;
				}
				else if (!m_flippedX && (playerBottomRight.x > pushableTopLeft.x))
				{
					pushableTopLeft.x = playerBottomRight.x;
				}

				//Match velocity
				m_currentPushable->m_velocity.x = m_velocity.x;
			}
		}
	}
}

void Player::AbilityGlide::OnEnterState()
{
	m_player.PlayAnimation(Animations::Player::colourSwitch);
}

void Player::AbilityGlide::OnUpdateState(float deltaTime)
{

}

void Player::AbilityGlide::OnExitState(State* newState)
{

}