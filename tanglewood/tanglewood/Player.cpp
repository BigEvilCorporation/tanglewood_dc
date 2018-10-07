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
#include "Globals.h"
#include "Animations.h"
#include "Palettes.h"
#include "framework/World.h"

#include "Flue.h"
#include "Mushroom.h"
#include "Fuzzl.h"
#include "TriggerBox.h"

const Palette* Player::s_colourPalettes[(int)ColourAbility::Count] =
{
	&Assets::Palettes::Player::red,
	&Assets::Palettes::Player::yellow,
	&Assets::Palettes::Player::green,
	&Assets::Palettes::Player::blue,
	&Assets::Palettes::Player::white
};

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

	m_minWallHeight = Constants::Player::minWallHeight;

	//Initial colour
	m_colour = ColourAbility::Red;

	//Use shared player palette
	SetPaletteTexture(Assets::Palettes::Player::shared);

	m_activeInteraction = InteractionType::None;
	m_activeAbility = nullptr;
	m_abilityTimer = 0.0f;
	m_paletteLerpSpeed = 0.0f;

	m_currentPushable = nullptr;

	TriggerBox::RegisterPotentialOccupant(*this);
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
	m_abilityState.AddState(new AbilityTimeSlow(*this), "timeslow");

	SwitchColour(ColourAbility::Green);
}

Player::~Player()
{
	m_world.RemoveEntity<Player>(*this);
	TriggerBox::UnregisterPotentialOccupant(*this);
	Flue::UnregisterPotentialOccupant(*this);
	Mushroom::UnregisterPotentialUser(*this);
}

void Player::Update(float deltaTime)
{
	Character::Update(deltaTime);

	//If jumping, ignore terrain holes
	m_ignoreHoles = m_jumping;

	//Update interaction
	switch (m_activeInteraction)
	{
	case InteractionType::Push:
		UpdatePushable();
		break;
	}

	//Update ability
	if (m_activeAbility)
	{
		m_abilityTimer -= deltaTime;
		if (m_abilityTimer <= 0.0f)
		{
			//Reset back to red
			SwitchColour(ColourAbility::Red);
		}
		else
		{
			m_abilityState.Update(deltaTime);
		}
	}

	//Update palette lerp
	UpdatePaletteLerp(deltaTime);
}

void Player::StartPaletteLerp(const Palette& source, const Palette& dest, float speed)
{
	m_sourcePalette = source;
	m_destPalette = dest;
	m_paletteLerpSpeed = speed;
	m_paletteLerpTimer = 0.0f;
}

void Player::UpdatePaletteLerp(float deltaTime)
{
	if (m_paletteLerpSpeed > 0.0f)
	{
		m_paletteLerpTimer += m_paletteLerpSpeed * deltaTime;

		if (m_paletteLerpTimer >= 1.0f)
		{
			m_paletteLerpTimer = 1.0f;
			m_paletteLerpSpeed = 0.0f;
		}

		Palette palette;
		PaletteTools::BlendPalettes(m_sourcePalette, m_destPalette, palette, m_paletteLerpTimer);
		PaletteTools::WritePaletteTexture(palette, Assets::Palettes::Player::shared);
	}
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

		if (TryInteractFuzzl())
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

void Player::BeginAbility()
{
	if (m_activeAbility)
	{
		m_activeAbility->BeginUse();
	}
}

void Player::EndAbility()
{
	if (m_activeAbility)
	{
		m_activeAbility->EndUse();
	}
}

void Player::SwitchColour(ColourAbility colour)
{
	//set new colour
	ColourAbility originalColour = m_colour;
	m_colour = colour;

	//End existing ability
	if (m_activeAbility)
	{
		EndAbility();
	}

	//Set new ability
	switch (colour)
	{
	case ColourAbility::Yellow:
		m_abilityState.SetState("glide");
		break;
	case ColourAbility::Green:
		m_abilityState.SetState("timeslow");
		break;
	default:
		m_abilityState.SetState(nullptr);
	}

	//Set active ability
	m_activeAbility = (Ability*)m_abilityState.GetCurrentState();

	//Red = return to normal
	if (colour != ColourAbility::Red)
	{
		//Play animation
		PlayAnimation(Animations::Player::colourSwitch);

		//Start timer
		m_abilityTimer = Constants::Player::colourAbilityMaxTime;
	}

	//Lerp palette
	StartPaletteLerp(*s_colourPalettes[(int)originalColour], *s_colourPalettes[(int)colour], Constants::Player::paletteLerpSpeed);
}

bool Player::TryInteractPushable()
{
	const std::vector<PhysicsObj*>& pushableObjs = m_world.GetPhysicsWorld().GetPushableObjects();

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

bool Player::TryInteractFuzzl()
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

}

void Player::AbilityGlide::OnUpdateState(float deltaTime)
{
	//Disable if on floor or heading upwards
	if (m_active && (m_player.m_closeToFloor || m_player.m_velocity.y >= 0.0f))
	{
		EndUse();
	}
}

void Player::AbilityGlide::OnExitState(State* newState)
{

}

void Player::AbilityGlide::BeginUse()
{
	//In not active, player in air, and heading downwards
	if (!m_active && !m_player.m_closeToFloor && m_player.m_velocity.y < 0.0f)
	{
		//Set glide animation and physics properties
		m_active = true;
		m_player.m_maxVelocityYDown = Constants::Player::maxVelocityYDownGlide;
		m_player.m_manualAnimation = true;
		m_player.PlayAnimation(Animations::Player::glide);
	}
}

void Player::AbilityGlide::EndUse()
{
	if (m_active)
	{
		//Reset animation and physics properties
		m_active = false;
		m_player.m_maxVelocityYDown = Constants::Character::maxVelocityYDown;
		m_player.m_manualAnimation = false;
	}
}

void Player::AbilityTimeSlow::OnEnterState()
{

}

void Player::AbilityTimeSlow::OnUpdateState(float deltaTime)
{

}

void Player::AbilityTimeSlow::OnExitState(State* newState)
{

}

void Player::AbilityTimeSlow::BeginUse()
{
	if (!m_active)
	{
		m_active = true;

		//Begin palette lerp
		m_originalPalette = Globals::Game::world->GetPalette();
		Palette grayscale;
		PaletteTools::GrayScalePalette(m_originalPalette, grayscale);
		Globals::Game::world->BeginPaletteLerp(grayscale, Constants::Player::timeSlowLerpSpeed);

		//Apply speed scale to all physics objects (except player)
		std::vector<PhysicsObj*> physicsObjs = Globals::Game::world->GetPhysicsWorld().GetPhysicsObjs();

		for (int i = 0; i < physicsObjs.size(); i++)
		{
			if (physicsObjs[i] != &m_player)
			{
				physicsObjs[i]->m_speedScale = 0.1f;
			}
		}
	}
}

void Player::AbilityTimeSlow::EndUse()
{
	if (m_active)
	{
		m_active = false;

		//Restore palette
		Globals::Game::world->BeginPaletteLerp(m_originalPalette, Constants::Player::timeSlowLerpSpeed);

		//Reset speed scale
		std::vector<PhysicsObj*> physicsObjs = Globals::Game::world->GetPhysicsWorld().GetPhysicsObjs();

		for (int i = 0; i < physicsObjs.size(); i++)
		{
			physicsObjs[i]->m_speedScale = 1.0f;
		}
	}
}