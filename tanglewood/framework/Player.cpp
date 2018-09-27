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
#include "World.h"

//TODO: Move to Nymn/Echo
#include "tanglewood/Flue.h"
#include "tanglewood/Mushroom.h"

Player::Player(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Character(world, gameObject, gameObjType, actor)
{
	m_boundsSize.x = Constants::Player::boundsWidth;
	m_boundsSize.y = Constants::Player::boundsHeight;
	m_boundsTopLeft.x = (m_size.x / 2.0f) - (Constants::Player::boundsWidth / 2);
	m_boundsTopLeft.y = (m_size.y / 2.0f) - (Constants::Player::boundsHeight / 2);
	m_boundsBottomRight.x = m_boundsTopLeft.x + Constants::Player::boundsWidth;
	m_boundsBottomRight.y = m_boundsTopLeft.y + Constants::Player::boundsHeight;

	m_activeInteraction = InteractionType::None;

	m_currentPushable = nullptr;

	Flue::RegisterPotentialOccupant(*this);
	Mushroom::RegisterPotentialUser(*this);

	//Setup animations
	m_characterAnimations[(int)CharacterAnimations::Idle] = std::make_pair("idle", "idle");
	m_characterAnimations[(int)CharacterAnimations::Dead] = std::make_pair("dead", "dead");
	m_characterAnimations[(int)CharacterAnimations::Run] = std::make_pair("run", "run");
	m_characterAnimations[(int)CharacterAnimations::Walk] = std::make_pair("walk", "walk");
	m_characterAnimations[(int)CharacterAnimations::Jump] = std::make_pair("jump", "jump");
	m_characterAnimations[(int)CharacterAnimations::Glide] = std::make_pair("glide", "glide");
	m_characterAnimations[(int)CharacterAnimations::Push] = std::make_pair("push", "push");
	m_characterAnimations[(int)CharacterAnimations::PushHeavy] = std::make_pair("pushHeavy", "pushHeavy");
	m_characterAnimations[(int)CharacterAnimations::Fall] = std::make_pair("fall", "fall");
	m_characterAnimations[(int)CharacterAnimations::WalkToRun] = std::make_pair("walktorun", "walktorun");
	m_characterAnimations[(int)CharacterAnimations::WaterWade] = std::make_pair("waterWade", "waterWade");
}

Player::~Player()
{
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
}

void Player::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	Character::Render(renderer, cameraInv, mapSize);
}

void Player::BeginInteract()
{
	if (m_activeInteraction == InteractionType::None)
	{
		if (FindPushable())
		{
			m_activeInteraction = InteractionType::Push;
			m_allowRunning = false;
			return;
		}
	}
}

void Player::EndInteract()
{
	m_activeInteraction = InteractionType::None;
	m_allowRunning = true;
	m_currentPushable = nullptr;
}

bool Player::FindPushable()
{
	const std::vector<PhysicsObj*>& pushableObjs = m_world.GetPushableObjects();

	//Find intersecting pushable obj
	for (int i = 0; i < pushableObjs.size() && !m_currentPushable; i++)
	{
		if (Intersects(*pushableObjs[i]))
		{
			m_currentPushable = pushableObjs[i];
		}
	}

	return m_currentPushable != nullptr;
}

void Player::UpdatePushable()
{
	if (m_currentPushable)
	{
		float playerCentre = GetWorldCentre().x;
		float pushableCentre = m_currentPushable->GetWorldCentre().x;

		//If facing right direction
		if (	(!m_flippedX && (pushableCentre > playerCentre))
			||	(m_flippedX && (pushableCentre < playerCentre)))
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