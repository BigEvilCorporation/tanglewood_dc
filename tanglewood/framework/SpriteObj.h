///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		SpriteObj.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Sprite object, animation and rendering
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
#include "Sprite.h"

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

#if USE_PALETTE_TEXTURES
	//Palette swap
	void SetPaletteTexture(ion::render::Texture* texture) { m_paletteTexture = texture; }
	ion::render::Texture* GetPaletteTexture() { return m_paletteTexture; }
#endif

	ion::Vector2 m_drawOffset;	//Sprite draw offset
	bool m_flippedX;			//Sprite flip X
	bool m_flippedY;			//Sprite flip Y
	bool m_invertFlipX;			//X flipping inverted (if sprite sheet exported wrong way)
	bool m_visible;				//Is visible
	bool m_drawnLastFrame;		//Was sprite drawn last frame

	PlanePriority m_planePriority;

private:
	//Read object vars
	void ReadVars(const std::vector<GameObjectVariable>& vars);

	const Sprite* m_sprite;
	const Sprite::Sheet* m_currentSheet;
	SpriteAnimation* m_currentAnim;
	const AnimType* m_currentAnimType;
	std::vector<AnimType> m_animQueue;

#if USE_PALETTE_TEXTURES
	ion::render::Texture* m_paletteTexture;
	ion::render::Texture* m_paletteTextureDefault;
#endif
};
