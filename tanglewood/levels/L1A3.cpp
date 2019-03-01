///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A3.cpp
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 3
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "L1A3.h"
#include "Globals.h"
#include "Palettes.h"
#include "framework/World.h"
#include "tanglewood/Djakk.h"

#include <ion/core/string/String.h>

L1A3::L1A3()
{
	TriggerBox::RegisterTriggerFunc("L1A3_Trigger_WakeDjakk", std::bind(&L1A3::OnTriggerDjakk, this, std::placeholders::_1));
	TriggerBox::RegisterTriggerFunc("L1A3_Trigger_End", std::bind(&L1A3::OnTriggerEndLevel, this, std::placeholders::_1));
}

L1A3::~L1A3()
{
	TriggerBox::UnregisterTriggerFunc("L1A3_Trigger_WakeDjakk");
	TriggerBox::UnregisterTriggerFunc("L1A3_Trigger_End");
}

void L1A3::Start()
{
	//Start at night time
	Globals::Game::world->SetPalette(Assets::Palettes::World::night);
}

void L1A3::Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad)
{

}

void L1A3::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{

}

void L1A3::End()
{

}

void L1A3::OnTriggerDjakk(const TriggerBox& triggerBox)
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

	ion::debug::Assert(djakk, "L1A3::OnTriggerDjakk() - Djakk not found");

	djakk->BeginChase(false);
}

void L1A3::OnTriggerEndLevel(const TriggerBox& triggerBox)
{
	m_state = Level::State::ActEnded;
}