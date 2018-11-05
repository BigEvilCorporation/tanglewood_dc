///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A2.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 2
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "L1A2.h"
#include "Globals.h"
#include "Palettes.h"
#include "framework/World.h"
#include "tanglewood/Djakk.h"

#include <ion/core/string/String.h>

L1A2::L1A2()
{
	TriggerBox::RegisterTriggerFunc("L1A2_Trigger_Dusk", std::bind(&L1A2::OnTriggerDusk, this, std::placeholders::_1));
	TriggerBox::RegisterTriggerFunc("L1A2_Trigger_Night", std::bind(&L1A2::OnTriggerNight, this, std::placeholders::_1));
	TriggerBox::RegisterTriggerFunc("L1A2_Trigger_Djakk", std::bind(&L1A2::OnTriggerDjakk, this, std::placeholders::_1));
	TriggerBox::RegisterTriggerFunc("L1A2_Trigger_End", std::bind(&L1A2::OnTriggerEndLevel, this, std::placeholders::_1));
}

L1A2::~L1A2()
{
	TriggerBox::UnregisterTriggerFunc("L1A2_Trigger_Dusk");
	TriggerBox::UnregisterTriggerFunc("L1A2_Trigger_Night");
	TriggerBox::UnregisterTriggerFunc("L1A2_Trigger_Djakk");
	TriggerBox::UnregisterTriggerFunc("L1A2_Trigger_End");
}

void L1A2::Start()
{
	m_cutsceneDjakkReveal = new Cutscenes::DjakkReveal(*Globals::Players::player1, *Globals::Game::world->FindEntity<Djakk>("djakk"));
}

void L1A2::Update(float deltaTime)
{
	m_cutsceneDjakkReveal->Update(deltaTime);
}

void L1A2::End()
{

}

void L1A2::OnTriggerDusk(const TriggerBox& triggerBox)
{
	Globals::Game::world->BeginPaletteLerp(Assets::Palettes::World::dusk, Constants::World::paletteLerpSpeed);
}

void L1A2::OnTriggerNight(const TriggerBox& triggerBox)
{
	Globals::Game::world->BeginPaletteLerp(Assets::Palettes::World::night, Constants::World::paletteLerpSpeed);
}

void L1A2::OnTriggerDjakk(const TriggerBox& triggerBox)
{
	m_cutsceneDjakkReveal->Play();
}

void L1A2::OnTriggerEndLevel(const TriggerBox& triggerBox)
{
	m_state = Level::State::ActEnded;
}