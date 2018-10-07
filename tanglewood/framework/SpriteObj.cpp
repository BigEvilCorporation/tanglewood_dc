///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		SpriteObj.cpp
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Sprite sheet loading, animation and rendering
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

	m_currentSheet = NULL;
	m_currentAnim = NULL;
	m_currentAnimType = NULL;

#if USE_PALETTE_TEXTURES
	m_paletteTexture = NULL;
	m_paletteTextureDefault = NULL;
#endif

	m_flippedX = false;
	m_flippedY = false;
	m_visible = true;
	m_drawnLastFrame = false;

	m_planePriority = PlanePriority::SpriteLow;

	if(actor)
	{
		LoadActor(*actor);
		
		if (m_sheets.size() > 0)
		{
			m_currentSheet = &m_sheets.begin()->second;
		}
	}

	ReadVars(gameObject.GetVariables());
}

SpriteObj::~SpriteObj()
{
	m_world.RemoveEntity<SpriteObj>(*this);

	for(std::map<std::string, Sheet>::iterator it = m_sheets.begin(), end = m_sheets.end(); it != end; it++)
	{
		for(int i = 0; i < it->second.m_frames.size(); i++)
		{
			delete it->second.m_frames[i].material;
			delete it->second.m_frames[i].texture;
		}

		delete it->second.m_primitive;
	}

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

void SpriteObj::LoadActor(Actor& actor)
{
	for(TSpriteSheetMap::iterator it = actor.SpriteSheetsBegin(), end = actor.SpriteSheetsEnd(); it != end; ++it)
	{
		LoadSheet(it->second);
	}
}

void SpriteObj::LoadSheet(SpriteSheet& spriteSheet)
{
	const int tileWidth = 8;
	const int tileHeight = 8;

	//Add to map
	Sheet& sheet = m_sheets[spriteSheet.GetName()];

	u32 widthTiles = spriteSheet.GetWidthTiles();
	u32 heightTiles = spriteSheet.GetHeightTiles();
	u32 quadWidth = widthTiles * tileWidth;
	u32 quadHeight = heightTiles * tileHeight;
	u32 textureWidth = ion::maths::NextPowerOfTwo(quadWidth);
	u32 textureHeight = ion::maths::NextPowerOfTwo(quadHeight);
	u32 bytesPerPixel = 4;
	u32 textureSize = textureWidth * textureHeight * bytesPerPixel;

	//Create primitive
	sheet.m_primitive = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2((float)spriteSheet.GetWidthTiles() * (tileWidth / 2.0f), (float)spriteSheet.GetHeightTiles() * (tileHeight / 2.0f)));

	//Set UV coords
	ion::render::TexCoord coords[4];

	const float top = (float)quadHeight / (float)textureHeight;
	const float left = 0.0f;
	const float bottom = 0.0f;
	const float right = (float)quadWidth / (float)textureWidth;

	//Top left
	coords[0].x = left;
	coords[0].y = top;
	//Bottom left
	coords[1].x = left;
	coords[1].y = bottom;
	//Bottom right
	coords[2].x = right;
	coords[2].y = bottom;
	//Top right
	coords[3].x = right;
	coords[3].y = top;

	sheet.m_primitive->SetTexCoords(coords);

	//Create all render frame textures
	for (int i = 0; i < spriteSheet.GetNumFrames(); i++)
	{
		//Create new render frame
		Sheet::Frame renderFrame;

		//Create tileset texture for frame
		renderFrame.texture = ion::render::Texture::Create(textureWidth, textureHeight);

		//Create material
		renderFrame.material = new ion::render::Material();
		renderFrame.material->AddDiffuseMap(renderFrame.texture);
		renderFrame.material->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f, 1.0f));

#if defined ION_RENDERER_SHADER
#if USE_PALETTE_TEXTURES
		renderFrame.material->SetVertexShader(Assets::Shaders::IndexTexture::vertexShader.Get());
		renderFrame.material->SetPixelShader(Assets::Shaders::IndexTexture::pixelShader.Get());
#else
		renderFrame.material->SetVertexShader(Assets::Shaders::Default::vertexShader.Get());
		renderFrame.material->SetPixelShader(Assets::Shaders::Default::pixelShader.Get());
#endif
#endif

		//Insert frame
		sheet.m_frames.push_back(renderFrame);

		//Enumerate anims
		for (TSpriteAnimMap::iterator it = spriteSheet.AnimationsBegin(), end = spriteSheet.AnimationsEnd(); it != end; ++it)
		{
			sheet.m_animations[it->second.GetName()] = &it->second;
		}
	}

	//Create palette texture
	m_paletteTextureDefault = PaletteTools::CreatePaletteTexture(spriteSheet.GetPalette());
	m_paletteTexture = m_paletteTextureDefault;

	//Paint sprite sheet
	PaintSheet(spriteSheet, spriteSheet.GetPalette());
}

void SpriteObj::PaintSheet(SpriteSheet& spriteSheet, const Palette& palette)
{
	Sheet& sheet = m_sheets[spriteSheet.GetName()];

	const int tileWidth = 8;
	const int tileHeight = 8;
	u32 widthTiles = spriteSheet.GetWidthTiles();
	u32 heightTiles = spriteSheet.GetHeightTiles();

	for (int i = 0; i < spriteSheet.GetNumFrames(); i++)
	{
		//Get spriteSheet frame
		const SpriteSheetFrame& spriteSheetFrame = spriteSheet.GetFrame(i);

		//Get render frame
		Sheet::Frame& renderFrame = sheet.m_frames[i];

		u32 bytesPerPixel = 4;
		u32 textureWidth = renderFrame.texture->GetWidth();
		u32 textureHeight = renderFrame.texture->GetHeight();
		u32 textureSize = textureWidth * textureHeight * bytesPerPixel;

		u8* data = new u8[textureSize];
		ion::memory::MemSet(data, 0, textureSize);

		for (int tileX = 0; tileX < spriteSheet.GetWidthTiles(); tileX++)
		{
			for (int tileY = 0; tileY < spriteSheet.GetHeightTiles(); tileY++)
			{
				//Genesis spriteSheet order = column major
				const Tile& tile = spriteSheetFrame[(tileX * heightTiles) + tileY];

				//Invert Y for OpenGL
				int tileY_inv = spriteSheet.GetHeightTiles() - 1 - tileY;

				//Paint tile to texture
				for (int pixelY = 0; pixelY < tileHeight; pixelY++)
				{
					for (int pixelX = 0; pixelX < tileWidth; pixelX++)
					{
						//Invert Y for OpenGL
						int pixelY_OGL = tileHeight - 1 - pixelY;

						u8 colourIdx = tile.GetPixelColour(pixelX, pixelY_OGL);

						int destPixelX = (tileX * tileWidth) + pixelX;
						int destPixelY = (tileY_inv * tileHeight) + pixelY;
						u32 pixelIdx = (destPixelY * textureWidth) + destPixelX;
						u32 dataOffset = pixelIdx * bytesPerPixel;
						ion::debug::Assert(dataOffset + 2 < textureSize, "eOut of bounds");

#if USE_PALETTE_TEXTURES
						data[dataOffset] = colourIdx;
#else
						const Colour& colour = palette.GetColour(colourIdx);
						data[dataOffset] = colour.GetRed();
						data[dataOffset + 1] = colour.GetGreen();
						data[dataOffset + 2] = colour.GetBlue();
						data[dataOffset + 3] = colourIdx > 0 ? 255 : 0;
#endif
					}
				}
			}
		}

		//Load texture data
		renderFrame.texture->Load(textureWidth, textureHeight, ion::render::Texture::eRGBA, ion::render::Texture::eRGBA, ion::render::Texture::eBPP24, false, false, data);

		//Reset filter
		renderFrame.texture->SetMinifyFilter(ion::render::Texture::eFilterNearest);
		renderFrame.texture->SetMagnifyFilter(ion::render::Texture::eFilterNearest);
		renderFrame.texture->SetWrapping(ion::render::Texture::eWrapClamp);

		delete[] data;
	}
}

void SpriteObj::SetSpriteSheet(const std::string& sheetName)
{
	std::map<std::string, Sheet>::iterator sheetIt = m_sheets.find(sheetName);
	if (sheetIt != m_sheets.end())
	{
		m_currentSheet = &sheetIt->second;
	}
	else
	{
		ion::debug::error << "Could not find sprite sheet " << sheetName << ion::debug::end;
	}
}

void SpriteObj::PlayAnimation(const AnimType& animation)
{
	std::map<std::string, Sheet>::iterator sheetIt = m_sheets.find(animation.sheetName);
	if(sheetIt != m_sheets.end())
	{
		std::map<std::string, SpriteAnimation*>::iterator animIt = sheetIt->second.m_animations.find(animation.animName);
		if(animIt != sheetIt->second.m_animations.end())
		{
			if(m_currentAnim != animIt->second)
			{
				m_currentSheet = &sheetIt->second;

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
			//Draw offset (centred quad to top-left + draw offset, inverted for OpenGL)
			ion::Matrix4 transform;
			transform.SetTranslation(ion::Vector3(m_worldPos.x + m_drawOffset.x + (m_size.x / 2.0f), mapSize.y - m_worldPos.y + m_drawOffset.y - (m_size.y / 2.0f), Constants::Rendering::planePriorities[(int)m_planePriority]));

			//Flip
			ion::Vector3 scale(m_flippedX ? -1.0f : 1.0f, m_flippedY ? -1.0f : 1.0f, 1.0f);
			transform.SetScale(scale);

			//Get current anim frame
			int spriteFrame = m_currentAnim ? m_currentAnim->m_trackSpriteFrame.GetValue(m_currentAnim->GetFrame()) : 0;

#if USE_PALETTE_TEXTURES
			Assets::Shaders::IndexTexture::Params::indexedTexture.SetValue(*m_currentSheet->m_frames[spriteFrame].texture);
			Assets::Shaders::IndexTexture::Params::paletteTexture.SetValue(*m_paletteTexture);
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
