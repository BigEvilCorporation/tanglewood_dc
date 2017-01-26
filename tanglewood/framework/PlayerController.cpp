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
    
}
    
void PlayerController::Update(float deltaTime, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad)
{
    //Update input
    float moveSpeed = gamepad.GetLeftStick().x;
    bool jump = gamepad.ButtonDown(ion::input::Gamepad::BUTTON_A);
    
    if(keyboard.KeyDown(ion::input::Keycode::LEFT))
    {
        moveSpeed = -1.0f;
    }
    else if(keyboard.KeyDown(ion::input::Keycode::RIGHT))
    {
        moveSpeed = 1.0f;
    }
    
    jump |= keyboard.KeyDown(ion::input::Keycode::SPACE);
    
#if defined DEBUG
    if(keyboard.KeyDown(ion::input::Keycode::UP))
    {
        m_player.m_worldPos.y += 896.0f * deltaTime;
    }
    if(keyboard.KeyDown(ion::input::Keycode::DOWN))
    {
        m_player.m_worldPos.y -= 896.0f * deltaTime;
    }
#endif
    
    m_player.Move(moveSpeed);
    
    if(jump)
    {
        m_player.Jump();
    }
}
    
ion::Vector2 PlayerController::GetCentre() const
{
    return ion::Vector2(m_player.m_worldPos.x + (m_player.m_size.x / 2.0f), m_player.m_worldPos.y - (m_player.m_size.y / 2.0f));
}
