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

#include "cutscenes/DjakkReveal.h"

class L1A2 : public Level
{
public:
	L1A2();
	virtual ~L1A2();

	virtual void Start();
	virtual void Update(float deltaTime, ion::input::Keyboard* keyboard, ion::input::Mouse* mouse, ion::input::Gamepad* gamepad);
	virtual void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport);
	virtual void End();

	void OnTriggerDusk(const TriggerBox& triggerBox);
	void OnTriggerNight(const TriggerBox& triggerBox);
	void OnTriggerDjakk(const TriggerBox& triggerBox);
	void OnTriggerEndLevel(const TriggerBox& triggerBox);

private:
	Cutscenes::DjakkReveal* m_cutsceneDjakkReveal;
};