///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Cutscene.cpp
// Date:		4th November 2018
// Authors:		Matt Phillips
// Description:	Cutscene system
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "Cutscene.h"

Cutscene::Cutscene()
{
	m_currentStep = -1;
	m_playing = false;
}

Cutscene::~Cutscene()
{
	for (int i = 0; i < m_steps.size(); i++)
	{
		delete m_steps[i];
	}
}

void Cutscene::Play()
{
	Setup();
	m_playing = true;
	m_currentStep = -1;
	AdvanceStep();
	m_letterbox.Begin();
}

void Cutscene::Stop()
{
	m_playing = false;
	m_letterbox.End();
}

void Cutscene::Update(float deltaTime)
{
	if (m_playing)
	{
		if (!m_steps[m_currentStep]->m_tick(deltaTime))
		{
			AdvanceStep();
		}
	}

	m_letterbox.Update(deltaTime);
}

void Cutscene::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, ion::render::Viewport& viewport)
{
	m_letterbox.Render(renderer);
}

void Cutscene::AddStep(Step* step)
{
	m_steps.push_back(step);
}

void Cutscene::AdvanceStep()
{
	m_currentStep++;

	if (m_currentStep < m_steps.size())
	{
		m_steps[m_currentStep]->m_start();
	}
	else
	{
		Stop();
	}
}
