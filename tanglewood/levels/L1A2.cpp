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

L1A2::L1A2()
{
	TriggerBox::RegisterTriggerFunc("L1A2_Trigger_End", std::bind(&L1A2::OnTriggerEndLevel, this, std::placeholders::_1));
}

L1A2::~L1A2()
{
	TriggerBox::UnregisterTriggerFunc("L1A2_Trigger_End");
}

void L1A2::Start()
{

}

void L1A2::Update(float deltaTime)
{

}

void L1A2::End()
{

}

void L1A2::OnTriggerEndLevel(const TriggerBox& triggerBox)
{
	EndLevel();
}