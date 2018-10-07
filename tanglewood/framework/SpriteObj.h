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

#if defined ION_RENDERER_SHADER
#include <ion/renderer/Shader.h>
#endif

#include "Entity.h"
#include "Constants.h"
#include "Animation.h"
#include "PlanePriority.h"

class SpriteObj : public Entity
{
public:
	SpriteObj(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor);
	virtual ~SpriteObj();

	//Set current sprite sheet
	void SetSpriteSheet(const std::string& sheetName);

	//Animation
	void PlayAnimation(const AnimType& animation);
	void QueueAnimation(const AnimType& animation);

	const AnimType* GetCurrentAnimType() const { return m_currentAnimType; }
	SpriteAnimation* GetCurrentAnimation() const { return m_currentAnim; }

	//Update/render
	virtual void Update(float deltaTime);
	virtual void Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, const ion::render::Viewport& viewport, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize);

	//Draw sprite sheet to texture with specified palette
	void PaintSheet(SpriteSheet& spriteSheet, const Palette& palette);

#if USE_PALETTE_TEXTURES
	//Palette swap
	void SetPaletteTexture(ion::render::Texture* texture) { m_paletteTexture = texture; }
#endif

	ion::Vector2 m_drawOffset;	//Sprite draw offset
	bool m_flippedX;			//Sprite flip X
	bool m_flippedY;			//Sprite flip Y
	bool m_visible;				//Is visible
	bool m_drawnLastFrame;		//Was sprite drawn last frame

	PlanePriority m_planePriority;

private:
	//Read object vars
	void ReadVars(const std::vector<GameObjectVariable>& vars);

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

	std::map<std::string, Sheet> m_sheets;
	Sheet* m_currentSheet;
	SpriteAnimation* m_currentAnim;
	const AnimType* m_currentAnimType;
	std::vector<AnimType> m_animQueue;

#if USE_PALETTE_TEXTURES
	ion::render::Texture* m_paletteTexture;
	ion::render::Texture* m_paletteTextureDefault;
#endif
};
