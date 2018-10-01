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

L1A1::L1A1(const LevelData& levelData)
	: Level(levelData)
{
	TriggerBox::RegisterTriggerFunc("L1A1_Trigger_End", std::bind(&L1A1::OnTriggerEndLevel, this, std::placeholders::_1));
}

L1A1::~L1A1()
{
	TriggerBox::UnregisterTriggerFunc("L1A1_Trigger_End");
}

void L1A1::Start()
{

}

void L1A1::Update(float deltaTime)
{

}

void L1A1::End()
{

}

void L1A1::OnTriggerEndLevel(const TriggerBox& triggerBox)
{
	EndLevel();
}