///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		PlayerController.h
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Applied keyboard/gamepad input to player
///////////////////////////////////////////////////////////////

#include "tanglewood/Player.h"

#include <ion/input/Keyboard.h>
#include <ion/input/Gamepad.h>

class PlayerController
{
public:
    PlayerController(Player& player);
    
    void Update(float deltaTime, const ion::input::Keyboard& keyboard, const ion::input::Gamepad& gamepad);
    
    ion::Vector2 GetCentre() const;

	Player& GetPlayer() const { return m_player; }

	bool m_debugMove;

private:
    Player& m_player;
};
