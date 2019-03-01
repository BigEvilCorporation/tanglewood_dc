///////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Common.h
// Date:		4th November 2018
// Authors:		Matt Phillips
// Description:	Common cutscene steps
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include "framework/Cutscene.h"
#include "framework/Camera.h"

namespace Cutscenes
{
	namespace Common
	{
		struct Delay : Cutscene::Step
		{
			Delay(float time)
				: Cutscene::Step("Delay", std::bind(&Delay::Start, this), std::bind(&Delay::Tick, this, std::placeholders::_1))
				, m_time(time)
			{

			}

			void Start()
			{
				m_timer = m_time;
			}

			bool Tick(float deltaTime)
			{
				m_timer -= deltaTime;
				return m_timer > 0.0f;
			}

			float m_timer;
			float m_time;
		};

		struct CameraLerp : Cutscene::Step
		{
			CameraLerp(GameCamera& camera, const Entity& entity, float speed, bool blocking)
				: Cutscene::Step("CameraLerp", std::bind(&CameraLerp::Start, this), std::bind(&CameraLerp::Tick, this, std::placeholders::_1))
				, m_camera(camera)
				, m_entity(entity)
				, m_speed(speed)
				, m_blocking(blocking)
			{

			}

			void Start()
			{
				m_camera.BeginLerp(&m_entity, m_speed);
			}

			bool Tick(float deltaTime)
			{
				return m_blocking && m_camera.IsLerping();
			}

			const Entity& m_entity;
			GameCamera& m_camera;
			float m_speed;
			bool m_blocking;
		};
	}
}