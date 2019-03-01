///////////////////////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
//
// File:		Camera.h
// Date:		4th November 2018
// Authors:		Matt Phillips
// Description:	The game camera, and camera effects
///////////////////////////////////////////////////////////////////

#pragma once

#include "Entity.h"

class GameCamera
{
public:
	GameCamera();

	void Update(float deltaTime, float worldHeight);

	void SetTarget(const Entity* entity);
	const Entity* GetTarget() const;

	void BeginLerp(const Entity* target, float speed);
	bool IsLerping() const;

	void SetWorldPos(const ion::Vector2& worldPos);
	const ion::Vector2& GetWorldPos() const;

	const ion::render::Camera& GetRenderCamera() const;

private:
	ion::render::Camera m_renderCamera;
	const Entity* m_targetEntity;
	ion::Vector2 m_worldPos;

	//Camera lerp
	ion::Vector2 m_lerpStartPoint;
	float m_lerpSpeed;
	float m_lerpTime;
};