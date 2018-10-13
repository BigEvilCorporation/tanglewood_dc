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
#include "Djakk.h"
#include "TriggerBox.h"
#include "DeathBox.h"

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

	m_currentMount = nullptr;
	m_currentPushable = nullptr;

	TriggerBox::RegisterPotentialOccupant(*this);
	DeathBox::RegisterPotentialVictim(*this);
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
	m_abilityState.AddState(new AbilityBeastTame(*this), "beasttame");
}

Player::~Player()
{
	m_world.RemoveEntity<Player>(*this);
	TriggerBox::UnregisterPotentialOccupant(*this);
	DeathBox::UnregisterPotentialVictim(*this);
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

	//Update saddle pos
	if (m_currentMount)
	{
		m_worldPos = m_currentMount->m_worldPos + m_mountSaddlePos;
		m_flippedX = m_currentMount->m_flippedX;
	}
}

void Player::Move(float speed)
{
	if (m_currentMount)
	{
		m_currentMount->Move(speed);
	}
	else
	{
		Character::Move(speed);
	}
}

void Player::Jump()
{
	if (m_currentMount)
	{
		m_currentMount->Jump();
	}
	else
	{
		Character::Jump();
	}
}

void Player::CancelJump()
{
	if (m_currentMount)
	{
		m_currentMount->CancelJump();
	}
	else
	{
		Character::CancelJump();
	}
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

void Player::BeginInteract(bool debounce)
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

void Player::BeginAbility(bool debounce)
{
	if (m_activeAbility)
	{
		m_activeAbility->BeginUse(debounce);
	}
}

void Player::EndAbility(bool timedOut)
{
	if (m_activeAbility)
	{
		m_activeAbility->EndUse(timedOut);
	}
}

void Player::SwitchColour(ColourAbility colour)
{
	//set new colour
	ColourAbility originalColour = m_colour;
	m_colour = colour;

	//End existing ability (timed out)
	if (m_activeAbility)
	{
		EndAbility(true);
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
	case ColourAbility::Blue:
		m_abilityState.SetState("beasttame");
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
		EndUse(false);
	}
}

void Player::AbilityGlide::OnExitState(State* newState)
{

}

void Player::AbilityGlide::BeginUse(bool debounce)
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

void Player::AbilityGlide::EndUse(bool timedOut)
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

void Player::AbilityTimeSlow::BeginUse(bool debounce)
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
				physicsObjs[i]->m_speedScale = Constants::Player::timeSlowPhysicsScale;
			}
		}
	}
}

void Player::AbilityTimeSlow::EndUse(bool timedOut)
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

void Player::AbilityBeastTame::OnEnterState()
{
	//Tame all Djakks
	const std::vector<Djakk*>& djakks = m_player.m_world.GetEntities<Djakk>();

	for (int i = 0; i < djakks.size(); i++)
	{
		djakks[i]->BeginTame();
	}
}

void Player::AbilityBeastTame::OnUpdateState(float deltaTime)
{
	if (m_mounting)
	{
		//Waiting for mount anim to finish
		if (!m_player.GetCurrentAnimation() || m_player.GetCurrentAnimation()->GetState() == ion::render::Animation::eStopped)
		{
			//Forward all controls to pet
			m_player.m_currentMount = m_beast;
			m_player.m_mountSaddlePos = Constants::Djakk::saddleOffset;

			m_mounting = false;
		}
	}
	else if(m_active)
	{
		//Override anim
		if (m_beast->m_jumping)
		{
			m_player.PlayAnimation(Animations::Player::rideJump);
		}
		else if (m_beast->m_running)
		{
			m_player.PlayAnimation(Animations::Player::rideGallop);
		}
		else
		{
			m_player.PlayAnimation(Animations::Player::rideTrot);
		}
		

		//Match Djakk anim frame
		m_player.GetCurrentAnimation()->SetFrame(m_beast->GetCurrentAnimation()->GetFrame());
	}
}

void Player::AbilityBeastTame::OnExitState(State* newState)
{
	//Un-tame all Djakks
	const std::vector<Djakk*>& djakks = m_player.m_world.GetEntities<Djakk>();

	for (int i = 0; i < djakks.size(); i++)
	{
		djakks[i]->EndTame();
	}
}

void Player::AbilityBeastTame::BeginUse(bool debounce)
{
	//Toggle
	if (debounce && !m_mounting)
	{
		if (m_active)
		{
			//End ride
			StopRiding(false);
		}
		else
		{
			//Find a Djakk
			const std::vector<Djakk*>& djakks = m_player.m_world.GetEntities<Djakk>();

			for (int i = 0; i < djakks.size() && !m_active; i++)
			{
				if (m_player.Intersects(*djakks[i]))
				{
					//Found one
					m_beast = djakks[i];
					m_beast->BeginRide(m_player);

					//Snap to mount position
					m_player.m_velocity = ion::Vector2();
					m_player.m_acceleration = ion::Vector2();
					m_player.m_worldPos.x = m_beast->m_worldPos.x + Constants::Djakk::saddleOffset.x;
					m_player.m_flippedX = m_beast->m_flippedX;

					//Mount up
					m_player.PlayAnimation(Animations::Player::mount);
					m_mounting = true;

					//Disable local physics and animation processing
					m_player.m_physicsEnabled = false;
					m_player.m_manualAnimation = true;

					m_active = true;
				}
			}
		}
	}
}

void Player::AbilityBeastTame::EndUse(bool timedOut)
{
	if (m_active && timedOut)
	{
		//End ride
		StopRiding(timedOut);

		//Player's getting kicked off
		m_player.m_velocity.x = m_player.m_flippedX ? -Constants::Player::beastTameTimeoutBuckVelocityX : Constants::Player::beastTameTimeoutBuckVelocityX;
		m_player.m_velocity.y = Constants::Player::beastTameTimeoutBuckVelocityY;
	}
}

void Player::AbilityBeastTame::StopRiding(bool buck)
{
	m_active = false;

	//Restore state
	m_player.m_currentMount = nullptr;
	m_player.m_physicsEnabled = true;
	m_player.m_manualAnimation = false;

	//End ride
	m_beast->EndRide(buck);

	//Jump off
	m_player.PlayAnimation(Animations::Player::jump);
	m_player.m_velocity.y = Constants::Character::jumpImpulse;
}