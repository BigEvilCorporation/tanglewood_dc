///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		SpriteObj.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Sprite object, animation and rendering
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include "SpriteObj.h"
#include "Constants.h"
#include "Globals.h"
#include "World.h"

#if defined ION_RENDERER_SHADER
#include "Shaders.h"
#endif

#include "framework/PaletteTools.h"

#include <ion/core/debug/Debug.h>
#include <ion/core/memory/Memory.h>
#include <ion/core/utils/STL.h>
#include <ion/core/string/String.h>
#include <ion/maths/Geometry.h>

SpriteObj::SpriteObj(World& world, const GameObject& gameObject, const GameObjectType& gameObjType, Actor* actor)
	: Entity(world, gameObject, gameObjType, actor)
{
	m_world.AddEntity<SpriteObj>(*this);

	m_sprite = nullptr;
	m_currentSheet = nullptr;
	m_currentAnim = nullptr;
	m_currentAnimType = nullptr;

#if USE_PALETTE_TEXTURES
	m_paletteTexture = nullptr;
	m_paletteTextureDefault = nullptr;
#endif

	m_flippedX = false;
	m_flippedY = false;
	m_invertFlipX = false;
	m_visible = true;
	m_drawnLastFrame = false;

	m_planePriority = PlanePriority::SpriteLow;

	ReadVars(gameObject.GetVariables());

	m_sprite = m_world.FindSprite(actor->GetName());

	if (m_sprite)
	{
		const SpriteSheet& spriteSheet = actor->GetSpriteSheets().begin()->second;

		m_currentSheet = m_sprite->FindSheet(spriteSheet.GetName());

#if USE_PALETTE_TEXTURES
		//Create palette texture
		m_paletteTextureDefault = PaletteTools::CreatePaletteTexture(spriteSheet.GetPalette());
		m_paletteTexture = m_paletteTextureDefault;
#endif
	}
}

SpriteObj::~SpriteObj()
{
	m_world.RemoveEntity<SpriteObj>(*this);

#if USE_PALETTE_TEXTURES
	if (m_paletteTextureDefault)
	{
		delete m_paletteTextureDefault;
	}
#endif
}

void SpriteObj::ReadVars(const std::vector<GameObjectVariable>& vars)
{
	for (int i = 0; i < vars.size(); i++)
	{
		if (ion::string::CompareNoCase(vars[i].m_name, "SpriteObj_FlippedX"))
		{
			if (vars[i].m_value == "0x1")
			{
				m_flippedX = true;
			}
		}
	}
}

void SpriteObj::SetSpriteSheet(const std::string& sheetName)
{
	m_currentSheet = m_sprite->FindSheet(sheetName);

	if (!m_currentSheet)
	{
		ion::debug::error << "Could not find sprite sheet " << sheetName << ion::debug::end;
	}
}

void SpriteObj::PlayAnimation(const AnimType& animation)
{
	if (m_sprite && (!m_currentAnimType || *m_currentAnimType != animation))
	{
		const Sprite::Sheet* sheet = m_sprite->FindSheet(animation.sheetName);
		if (sheet)
		{
			std::map<std::string, const SpriteAnimation*>::const_iterator animIt = sheet->m_animations.find(animation.animName);
			if (animIt != sheet->m_animations.end())
			{
				if (m_currentAnim != animIt->second)
				{
					m_currentSheet = sheet;

					if (!m_currentAnim || m_currentAnim->GetName() != animIt->second->GetName())
					{
						if (m_currentAnim)
						{
							delete m_currentAnim;
						}

						m_currentAnimType = &animation;
						m_currentAnim = new SpriteAnimation(*animIt->second);
						m_currentAnim->SetStart();
						m_currentAnim->SetPlaybackBehaviour((animation.flags & AnimFlags::Loop) ? ion::render::Animation::eLoop : ion::render::Animation::ePlayOnce);
					}

					//Set speed
					m_currentAnim->SetPlaybackSpeed(m_currentAnim->GetPlaybackSpeed());

					//Begin playback
					m_currentAnim->SetState(ion::render::Animation::ePlaying);
				}
			}
			else
			{
				ion::debug::error << "Could not find animation " << animation.animName << " in sprite sheet " << animation.sheetName << ion::debug::end;
			}
		}
		else
		{
			ion::debug::error << "Could not find sprite sheet " << animation.sheetName << ion::debug::end;
		}
	}
}

void SpriteObj::QueueAnimation(const AnimType& animation)
{
	m_animQueue.push_back(animation);
}

void SpriteObj::Update(float deltaTime)
{
	//Update current anim
	if(m_currentAnim)
	{
		m_currentAnim->Update(deltaTime);
	}

	//Update queue
	if (!m_animQueue.empty())
	{
		if (!m_currentAnim || m_currentAnim->GetState() == ion::render::Animation::eStopped)
		{
			//Current anim stopped, pop front of queue
			PlayAnimation(m_animQueue.front());
			m_animQueue.erase(m_animQueue.begin());
		}
	}

	Entity::Update(deltaTime);
}

void SpriteObj::Render(ion::render::Renderer& renderer, const ion::render::Camera& camera, const ion::render::Viewport& viewport, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	if(m_currentSheet && m_visible)
	{
		//Visibility test
		ion::Vector2 topLeft;
		ion::Vector2 bottomRight;
		ion::Vector2 camTopLeft;
		ion::Vector2 camBottomRight;

		GetWorldBounds(topLeft, bottomRight);

		camTopLeft.x = camera.GetPosition().x;
		camTopLeft.y = mapSize.y - camera.GetPosition().y - Constants::MegaDrive::screenHeight;
		camBottomRight.x = camera.GetPosition().x + Constants::MegaDrive::screenWidth;
		camBottomRight.y = mapSize.y - camera.GetPosition().y;

		m_drawnLastFrame = ion::maths::BoxIntersectsBox(topLeft, bottomRight, camTopLeft, camBottomRight);

		if(m_drawnLastFrame)
		{
			//Get current anim frame
			int spriteFrame = m_currentAnim ? m_currentAnim->m_trackSpriteFrame.GetValue(m_currentAnim->GetFrame()) : 0;

			//Get curren anim pos offset
			ion::Vector2i animOffset = m_currentAnim ? m_currentAnim->m_trackPosition.GetValue(m_currentAnim->GetFrame()) : ion::Vector2i();

			//Draw offset (centred quad to top-left + draw offset, inverted for OpenGL)
			ion::Matrix4 transform;
			transform.SetTranslation(ion::Vector3(	m_worldPos.x + m_drawOffset.x + animOffset.x + (m_size.x / 2.0f),
													mapSize.y - m_worldPos.y + m_drawOffset.y + animOffset.y - (m_size.y / 2.0f),
													Constants::Rendering::planePriorities[(int)m_planePriority]));

			//Flip
			const bool flippedX = (m_flippedX && !m_invertFlipX) || (!m_flippedX && m_invertFlipX);
			ion::Vector3 scale(flippedX ? -1.0f : 1.0f, m_flippedY ? -1.0f : 1.0f, 1.0f);
			transform.SetScale(scale);

#if defined ION_RENDERER_SHADER
#if USE_PALETTE_TEXTURES
			Assets::Shaders::IndexTexture::Params::indexedTexture.SetValue(*m_currentSheet->m_frames[spriteFrame].texture);
			Assets::Shaders::IndexTexture::Params::paletteTexture.SetValue(*m_paletteTexture);
#endif
#endif

			//Bind material
			m_currentSheet->m_frames[spriteFrame].material->Bind(transform, cameraInv, renderer.GetProjectionMatrix());

			//Draw vertex buffer
			renderer.DrawVertexBuffer(m_currentSheet->m_primitive->GetVertexBuffer(), m_currentSheet->m_primitive->GetIndexBuffer());

			//Unbind material
			m_currentSheet->m_frames[spriteFrame].material->Unbind();
		}
	}
	else
	{
		m_drawnLastFrame = false;
	}

	Entity::Render(renderer, camera, viewport, cameraInv, mapSize);
}
