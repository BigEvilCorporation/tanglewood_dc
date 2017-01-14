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

#include <ion/core/debug/Debug.h>
#include <ion/core/memory/Memory.h>

SpriteObj::SpriteObj(const World& world, const GameObject& gameObject, const GameObjectType& gameObjType)
	: Entity(world, gameObject, gameObjType)
{
	m_currentSheet = NULL;
	m_currentAnim = NULL;

	m_flippedX = false;
	m_flippedY = false;
	m_visible = true;
	m_drawnLastFrame = false;
}

SpriteObj::~SpriteObj()
{
	for(std::map<std::string, Sheet>::iterator it = m_sheets.begin(), end = m_sheets.end(); it != end; it++)
	{
		for(int i = 0; i < it->second.m_frames.size(); i++)
		{
			delete it->second.m_frames[i].material;
			delete it->second.m_frames[i].texture;
		}

		delete it->second.m_primitive;
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

	const Palette& palette = spriteSheet.GetPalette();

	for(int i = 0; i < spriteSheet.GetNumFrames(); i++)
	{
		//Get spriteSheet frame
		const SpriteSheetFrame& spriteSheetFrame = spriteSheet.GetFrame(i);

		//Create new render frame
		Sheet::Frame renderFrame;

		//Create tileset texture for frame
		renderFrame.texture = ion::render::Texture::Create(textureWidth, textureHeight);

		u8* data = new u8[textureSize];
		ion::memory::MemSet(data, 0, textureSize);

		for(int tileX = 0; tileX < spriteSheet.GetWidthTiles(); tileX++)
		{
			for(int tileY = 0; tileY < spriteSheet.GetHeightTiles(); tileY++)
			{
				//Genesis spriteSheet order = column major
				const Tile& tile = spriteSheetFrame[(tileX * heightTiles) + tileY];

				//Invert Y for OpenGL
				int tileY_inv = spriteSheet.GetHeightTiles() - 1 - tileY;

				//Paint tile to texture
				for(int pixelY = 0; pixelY < tileHeight; pixelY++)
				{
					for(int pixelX = 0; pixelX < tileWidth; pixelX++)
					{
						//Invert Y for OpenGL
						int pixelY_OGL = tileHeight - 1 - pixelY;

						u8 colourIdx = tile.GetPixelColour(pixelX, pixelY_OGL);

						const Colour& colour = palette.GetColour(colourIdx);

						int destPixelX = (tileX * tileWidth) + pixelX;
						int destPixelY = (tileY_inv * tileHeight) + pixelY;
						u32 pixelIdx = (destPixelY * textureWidth) + destPixelX;
						u32 dataOffset = pixelIdx * bytesPerPixel;
						ion::debug::Assert(dataOffset + 2 < textureSize, "eOut of bounds");
						data[dataOffset] = colour.GetRed();
						data[dataOffset + 1] = colour.GetGreen();
						data[dataOffset + 2] = colour.GetBlue();
						data[dataOffset + 3] = colourIdx > 0 ? 255 : 0;
					}
				}
			}
		}

		renderFrame.texture->Load(textureWidth, textureHeight, ion::render::Texture::eRGBA, ion::render::Texture::eRGBA, ion::render::Texture::eBPP24, false, data);
		renderFrame.texture->SetMinifyFilter(ion::render::Texture::eFilterNearest);
		renderFrame.texture->SetMagnifyFilter(ion::render::Texture::eFilterNearest);
		renderFrame.texture->SetWrapping(ion::render::Texture::eWrapClamp);

		//Create material
		renderFrame.material = new ion::render::Material();
		renderFrame.material->AddDiffuseMap(renderFrame.texture);
		renderFrame.material->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f));

#if defined ION_RENDERER_SHADER
		renderFrame.material->SetVertexShader(vertexShader);
		renderFrame.material->SetPixelShader(pixelshader);
#endif

		//Insert frame
		sheet.m_frames.push_back(renderFrame);

		//Enumerate anims
		for(TSpriteAnimMap::iterator it = spriteSheet.AnimationsBegin(), end = spriteSheet.AnimationsEnd(); it != end; ++it)
		{
			sheet.m_animations[it->second.GetName()] = &it->second;
		}

		delete data;
	}
}
void SpriteObj::SetAnimation(const std::string& sheetName, const std::string& animName)
{
	std::map<std::string, Sheet>::iterator sheetIt = m_sheets.find(sheetName);
	if(sheetIt != m_sheets.end())
	{
		std::map<std::string, SpriteAnimation*>::iterator animIt = sheetIt->second.m_animations.find(animName);
		if(animIt != sheetIt->second.m_animations.end())
		{
			m_currentSheet = &sheetIt->second;
			m_currentAnim = animIt->second;

			//Set speed
			m_currentAnim->SetPlaybackSpeed(m_currentAnim->GetSpeed());

			//Begin playback
			m_currentAnim->SetState(ion::render::Animation::ePlaying);
		}
		else
		{
			ion::debug::error << "Could not find animation " << animName << " in sprite sheet " << sheetName << ion::debug::end;
		}
	}
	else
	{
		ion::debug::error << "Could not find sprite sheet " << sheetName << ion::debug::end;
	}
}

void SpriteObj::Update(float deltaTime)
{
	if(m_currentAnim)
	{
		const float frameRateMegaDrive = 24.0f;
		const float frameRateMul = 1.0f / (frameRateMegaDrive / 10.0f);
		float animDelta = ((float)m_currentAnim->GetSpeed() / 100.0f) * frameRateMul;

		m_currentAnim->Update(animDelta * deltaTime);
	}

	Entity::Update(deltaTime);
}

void SpriteObj::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv, const ion::Vector2& mapSize)
{
	if(m_currentSheet && m_currentAnim)
	{
		//TODO: Visibility test
		if(true)
		{
			//Draw offset (centred quad to top-left + draw offset, inverted for OpenGL)
			ion::Matrix4 transform;
			transform.SetTranslation(ion::Vector3(m_worldPos.x + m_drawOffset.x + (m_size.x / 2.0f), mapSize.y - m_worldPos.y + m_drawOffset.y - (m_size.y / 2.0f), 0.0f));

			//Flip
			ion::Vector3 scale(m_flippedX ? -1.0f : 1.0f, m_flippedY ? -1.0f : 1.0f, 1.0f);
			transform.SetScale(scale);

			//Set matrix
			renderer.SetMatrix(transform * cameraInv);

			//Get current anim frame
			int spriteFrame = m_currentAnim->m_trackSpriteFrame.GetValue(m_currentAnim->GetFrame());

			//Bind material
			m_currentSheet->m_frames[spriteFrame].material->Bind(transform, cameraInv, renderer.GetProjectionMatrix());

			//Draw vertex buffer
			renderer.DrawVertexBuffer(m_currentSheet->m_primitive->GetVertexBuffer(), m_currentSheet->m_primitive->GetIndexBuffer());
		}
	}

	Entity::Render(renderer, cameraInv, mapSize);
}