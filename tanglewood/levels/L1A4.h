///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A4.h
// Date:		7th October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 4
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Level.h"
#include "tanglewood/TriggerBox.h"

class L1A4 : public Level
{
public:
	L1A4();
	virtual ~L1A4();

	virtual void Start();
	virtual void Update(float deltaTime);
	virtual void End();

	void OnTriggerDjakk(const TriggerBox& triggerBox);
	void OnTriggerDismountDjakk(const TriggerBox& triggerBox);
	void OnTriggerEndLevel(const TriggerBox& triggerBox);
};