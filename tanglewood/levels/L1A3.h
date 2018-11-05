///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A3.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 3
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Level.h"
#include "tanglewood/TriggerBox.h"

class L1A3 : public Level
{
public:
	L1A3();
	virtual ~L1A3();

	virtual void Start();
	virtual void Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport);
	virtual void End();

	void OnTriggerDjakk(const TriggerBox& triggerBox);
	void OnTriggerEndLevel(const TriggerBox& triggerBox);
};