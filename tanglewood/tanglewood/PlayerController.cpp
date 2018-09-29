///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PlayerController.h
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Applied keyboard/gamepad input to player
///////////////////////////////////////////////////////////////

#include "PlayerController.h"

PlayerController::PlayerController(Player& player)
: m_player(player)
{
	m_debugMove = false;
}
    
void PlayerController::Update(float deltaTime, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad)
{
#if defined DEBUG
	if (keyboard.KeyPressedThisFrame(ion::input::Keycode::TAB))
	{
		m_debugMove = !m_debugMove;
	}

	if (m_debugMove)
	{
		float moveSpeed = 200;

		if (keyboard.KeyDown(ion::input::Keycode::N1))
		{
			moveSpeed *= 2.0f;
		}

		if (keyboard.KeyDown(ion::input::Keycode::N2))
		{
			moveSpeed *= 2.0f;
		}

		if (keyboard.KeyDown(ion::input::Keycode::N3))
		{
			moveSpeed *= 2.0f;
		}

		if (keyboard.KeyDown(ion::input::Keycode::UP))
		{
			m_player.m_worldPos.y -= moveSpeed * deltaTime;
		}

		if (keyboard.KeyDown(ion::input::Keycode::DOWN))
		{
			m_player.m_worldPos.y += moveSpeed * deltaTime;
		}

		if (keyboard.KeyDown(ion::input::Keycode::LEFT))
		{
			m_player.m_worldPos.x -= moveSpeed * deltaTime;
		}

		if (keyboard.KeyDown(ion::input::Keycode::RIGHT))
		{
			m_player.m_worldPos.x += moveSpeed * deltaTime;
		}
	}
	else
#endif
	{
		//Update input
		float moveSpeed = gamepad.GetLeftStick().x;

		bool interact = gamepad.ButtonDown(ion::input::GamepadButtons::BUTTON_X)
			|| keyboard.KeyDown(ion::input::Keycode::A);

		bool ability = gamepad.ButtonDown(ion::input::GamepadButtons::BUTTON_A)
			|| keyboard.KeyDown(ion::input::Keycode::S);

		bool jump = gamepad.ButtonDown(ion::input::GamepadButtons::BUTTON_B)
			|| keyboard.KeyDown(ion::input::Keycode::D);

		if (keyboard.KeyDown(ion::input::Keycode::LEFT))
		{
			moveSpeed = -1.0f;
		}
		else if (keyboard.KeyDown(ion::input::Keycode::RIGHT))
		{
			moveSpeed = 1.0f;
		}

		m_player.Move(moveSpeed);

		if (jump)
		{
			m_player.Jump();
		}

		if (interact)
			m_player.BeginInteract();
		else
			m_player.EndInteract();

		if (ability)
			m_player.BeginAbility();
		else
			m_player.EndAbility();
	}
}

ion::Vector2 PlayerController::GetCentre() const
{
	return m_player.GetWorldCentre();
}
