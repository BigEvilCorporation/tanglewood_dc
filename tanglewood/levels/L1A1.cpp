///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A1.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 1
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "L1A1.h"

#include "Animations.h"
#include "Globals.h"

#include "tanglewood/Player.h"
#include "tanglewood/PlayerController.h"

L1A1::L1A1()
{
	TriggerBox::RegisterTriggerFunc("L1A1_Trigger_End", std::bind(&L1A1::OnTriggerEndLevel, this, std::placeholders::_1));
}

L1A1::~L1A1()
{
	TriggerBox::UnregisterTriggerFunc("L1A1_Trigger_End");
}

void L1A1::Start()
{
	//Begin asleep
	Globals::Players::player1->PlayAnimation(Animations::Player::sleep);
	Globals::Players::player1->m_manualAnimation = true;
	m_awake = false;
}

void L1A1::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	if (!m_awake)
	{
		if (Globals::Players::playerController1->GetMoveSpeed() != 0.0f)
		{
			//Wake up
			Globals::Players::player1->PlayAnimation(Animations::Player::awake);
			Globals::Players::player1->m_manualAnimation = false;
			m_awake = true;
		}
	}
}

void L1A1::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}

void L1A1::End()
{

}

void L1A1::OnTriggerEndLevel(const TriggerBox& triggerBox)
{
	m_state = Level::State::ActEnded;
}