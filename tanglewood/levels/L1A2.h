///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A2.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 2
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Level.h"
#include "tanglewood/TriggerBox.h"

class L1A2 : public Level
{
public:
	L1A2();
	virtual ~L1A2();

	virtual void Start();
	virtual void Update(float deltaTime);
	virtual void End();

	void OnTriggerEndLevel(const TriggerBox& triggerBox);
};