///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		SpriteObj.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Sprite sheet loading, animation and rendering
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Primitive.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/beehive/SpriteSheet.h>
#include <ion/beehive/SpriteAnimation.h>

#include "Entity.h"

class SpriteObj : public Entity
{
public:
	SpriteObj(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~SpriteObj();

	//Set current sprite sheet
	void SetSpriteSheet(const std::string& sheetName);

	//Set current animation and begin playback
	void SetAnimation(const std::string& sheetName, const std::string& animName, bool loop);
	SpriteAnimation* GetCurrentAnimation() const { return m_currentAnim; }

	//Queue next animation (interrupts current looping anim)
	void QueueAnimation(const std::string& sheetName, const std::string& animName, bool loop);

	//Update/render
	virtual void Update(float deltaTime);
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);

	ion::Vector2 m_drawOffset;	//Sprite draw offset
	bool m_flippedX;			//Sprite flip X
	bool m_flippedY;			//Sprite flip Y
	bool m_visible;				//Is visible
	bool m_drawnLastFrame;		//Was sprite drawn last frame

private:
	//Load actor from Beehive data
	void LoadActor(Actor& actor);

	//Load sprite sheet from Beehive data
	void LoadSheet(SpriteSheet& spriteSheet);

	struct Sheet
	{
		struct Frame
		{
			ion::render::Texture* texture;
			ion::render::Material* material;
		};

		ion::render::Quad* m_primitive;
		std::vector<Frame> m_frames;
		std::map<std::string, SpriteAnimation*> m_animations;
	};

	struct QueuedAnim
	{
		std::string sheetName;
		std::string animName;
		bool looping;
	};
	
	std::map<std::string, Sheet> m_sheets;
	Sheet* m_currentSheet;
	SpriteAnimation* m_currentAnim;
	std::vector<QueuedAnim> m_animQueue;
};
