///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Cutscene.h
// Date:		4th November 2018
// Authors:		Matt Phillips
// Description:	Cutscene system
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Renderer.h>
#include <ion/renderer/Camera.h>
#include <ion/renderer/Viewport.h>

#include <vector>
#include <string>
#include <functional>

#include "effects/Letterbox.h"

class Cutscene
{
public:
	struct Step
	{
		Step() {}
		Step(const std::string& name, std::function<void()> const& start, std::function<bool(float deltaTime)> const& tick)
			: m_name(name)
			, m_start(start)
			, m_tick(tick)
		{}

		std::string m_name;
		std::function<void()> m_start;
		std::function<bool(float deltaTime)> m_tick;
	};

	Cutscene();
	virtual ~Cutscene();

	virtual void Setup() = 0;

	void Play();
	void Update(float deltaTime);
	void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport);

protected:
	void AddStep(Step* step);

private:
	void AdvanceStep();
	void Stop();

	std::vector<Step*> m_steps;
	int m_currentStep;
	bool m_playing;

	Letterbox m_letterbox;
};
