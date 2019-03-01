///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A4.cpp
// Date:		7th October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 4
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "L1A4.h"
#include "Globals.h"
#include "Palettes.h"
#include "framework/World.h"
#include "tanglewood/Djakk.h"

#include <ion/core/string/String.h>

L1A4::L1A4()
{
	TriggerBox::RegisterTriggerFunc("L1A4_Trigger_WakeDjakk", std::bind(&L1A4::OnTriggerDjakk, this, std::placeholders::_1));
	TriggerBox::RegisterTriggerFunc("L1A4_Trigger_DismountDjakk", std::bind(&L1A4::OnTriggerDismountDjakk, this, std::placeholders::_1));
	TriggerBox::RegisterTriggerFunc("L1A4_Trigger_End", std::bind(&L1A4::OnTriggerEndLevel, this, std::placeholders::_1));
}

L1A4::~L1A4()
{
	TriggerBox::UnregisterTriggerFunc("L1A4_Trigger_WakeDjakk");
	TriggerBox::UnregisterTriggerFunc("L1A4_Trigger_End");

	delete m_cutsceneDjakkReveal;
}

void L1A4::Start()
{
	//Start at night time
	Globals::Game::world->SetPalette(Assets::Palettes::World::night);

	m_cutsceneDjakkReveal = new Cutscenes::DjakkReveal(*Globals::Players::player1, *Globals::Game::world->FindEntity<Djakk>("djakk"));
}

void L1A4::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{
	m_cutsceneDjakkReveal->Update(deltaTime);
}

void L1A4::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{
	m_cutsceneDjakkReveal->Render(renderer, camera, viewport);
}

void L1A4::End()
{

}

void L1A4::OnTriggerDjakk(const TriggerBox& triggerBox)
{
	m_cutsceneDjakkReveal->Play();
}

void L1A4::OnTriggerDismountDjakk(const TriggerBox& triggerBox)
{
	Globals::Players::player1->EndAbility(true);
}

void L1A4::OnTriggerEndLevel(const TriggerBox& triggerBox)
{
	Globals::Players::player1->GoToSleep();
	m_state = Level::State::ChapterEnded;
}