///////////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Camera.cpp
// Date:		4th November 2018
// Authors:		Matt Phillips
// Description:	The game camera, and camera effects
///////////////////////////////////////////////////////////////////

#include "Camera.h"
#include "Globals.h"

GameCamera::GameCamera()
{
	//Calc ratio of window to screen size
	ion::Vector3 cameraZoom;
	cameraZoom.x = (float)Globals::Rendering::windowWidth / (float)Globals::Rendering::gameCanvasWidth;
	cameraZoom.y = (float)Globals::Rendering::windowHeight / (float)Globals::Rendering::gameCanvasHeight;
	cameraZoom.z = 1.0f;

	//Set camera zoom
	m_renderCamera.SetZoom(cameraZoom);
}

void GameCamera::Update(float deltaTime, float worldHeight)
{
	if (m_targetEntity)
	{
		//Update lerping
		if (m_lerpSpeed > 0.0f)
		{
			ion::Vector2 lerpTarget = m_targetEntity->GetWorldCentre();
			m_worldPos = m_lerpStartPoint.Lerp(lerpTarget, m_lerpTime);
			m_lerpTime += m_lerpSpeed * deltaTime;

			if (m_lerpTime >= 1.0f)
			{
				m_lerpSpeed = 0.0f;
			}
		}
		else
		{
			m_worldPos = m_targetEntity->GetWorldCentre();
		}
	}

	//Calc camera render pos
	ion::Vector3 cameraPos;
	cameraPos.x = m_worldPos.x - (float)Globals::Rendering::gameCanvasWidth / 2.0f;
	cameraPos.y = worldHeight - m_worldPos.y - (float)Globals::Rendering::gameCanvasHeight / 2.0f;
	cameraPos.z = -0.1f;

	//Set camera pos
	m_renderCamera.SetPosition(cameraPos);
}

void GameCamera::SetTarget(const Entity* entity)
{
	m_targetEntity = entity;
}

void GameCamera::BeginLerp(const Entity* target, float speed)
{
	m_lerpStartPoint = m_worldPos;
	m_targetEntity = target;
	m_lerpSpeed = speed;
	m_lerpTime = 0.0f;
}

bool GameCamera::IsLerping() const
{
	return m_lerpSpeed != 0.0f;
}

void GameCamera::SetWorldPos(const ion::Vector2& worldPos)
{
	m_worldPos = worldPos;
}

const ion::render::Camera& GameCamera::GetRenderCamera() const
{
	return m_renderCamera;
}

const ion::Vector2& GameCamera::GetWorldPos() const
{
	return m_worldPos;
}