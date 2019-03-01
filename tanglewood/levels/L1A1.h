///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		L1A1.h
// Date:		1st October 2018
// Authors:		Matt Phillips
// Description:	Chapter 1 Harlequin Forest Act 1
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Level.h"
#include "tanglewood/TriggerBox.h"

class L1A1 : public Level
{
public:
	L1A1();
	virtual ~L1A1();

	virtual void Start();
	virtual void Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport);
	virtual void End();

	void OnTriggerEndLevel(const TriggerBox& triggerBox);

private:
	bool m_awake;
};