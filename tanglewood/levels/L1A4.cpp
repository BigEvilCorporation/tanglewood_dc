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
}

void L1A4::Start()
{
	//Start at night time
	Globals::Game::world->SetPalette(Assets::Palettes::World::night);
}

void L1A4::Update(float deltaTime)
{

}

void L1A4::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}

void L1A4::End()
{

}

void L1A4::OnTriggerDjakk(const TriggerBox& triggerBox)
{
	std::vector<Djakk*> djakks = Globals::Game::world->GetEntities<Djakk>();
	
	Djakk* djakk = nullptr;

	for (int i = 0; i < djakks.size() && !djakk; i++)
	{
		if (ion::string::CompareNoCase(djakks[i]->m_name, "djakk"))
		{
			djakk = djakks[i];
		}
	}

	ion::debug::Assert(djakk, "L1A4::OnTriggerDjakk() - Djakk not found");

	djakk->BeginChase(false);
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