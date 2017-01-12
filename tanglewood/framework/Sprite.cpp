#include "Sprite.h"

#include <ion/core/debug/Debug.h>

Sprite::Sprite()
{
	m_currentSheet = NULL;
	m_currentAnim = NULL;
}

void Sprite::LoadActor(Actor& actor)
{
	for(TSpriteSheetMap::iterator it = actor.SpriteSheetsBegin(), end = actor.SpriteSheetsEnd(); it != end; ++it)
	{
		LoadSheet(it->second);
	}
}

void Sprite::LoadSheet(SpriteSheet& spriteSheet)
{
	const int tileWidth = 8;
	const int tileHeight = 8;

	//Add to map
	Sheet& sheet = m_sheets[spriteSheet.GetName()];

	sheet.m_primitive = new ion::render::Chessboard(ion::render::Chessboard::xy, ion::Vector2((float)spriteSheet.GetWidthTiles() * (tileWidth / 2.0f), (float)spriteSheet.GetHeightTiles() * (tileHeight / 2.0f)), spriteSheet.GetWidthTiles(), spriteSheet.GetHeightTiles(), true);

	u32 widthTiles = spriteSheet.GetWidthTiles();
	u32 heightTiles = spriteSheet.GetHeightTiles();
	u32 textureWidth = ion::maths::NextPowerOfTwo(widthTiles * tileWidth);
	u32 textureHeight = ion::maths::NextPowerOfTwo(heightTiles * tileHeight);
	u32 bytesPerPixel = 4;
	u32 textureSize = textureWidth * textureHeight * bytesPerPixel;

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

				//Set UV coords on primitive
				ion::render::TexCoord coords[4];
				ion::Vector2 textureBottomLeft((1.0f / (float)widthTiles) * tileX, (1.0f / (float)heightTiles) * tileY);

				float top = (textureBottomLeft.y + (1.0f / (float)heightTiles)) * ((float)(widthTiles * tileWidth) / (float)textureWidth);
				float left = textureBottomLeft.x;
				float bottom = textureBottomLeft.y * ((float)(heightTiles * tileHeight) / (float)textureHeight);
				float right = textureBottomLeft.x + (1.0f / (float)widthTiles);

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

				sheet.m_primitive->SetTexCoords((tileY * widthTiles) + tileX, coords);
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
void Sprite::SetAnimation(const std::string& sheetName, const std::string& animName)
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

void Sprite::Update(float deltaTime)
{
	if(m_currentAnim)
	{
		const float frameRateMegaDrive = 24.0f;
		const float frameRateMul = 1.0f / (frameRateMegaDrive / 10.0f);
		float animDelta = ((float)m_currentAnim->GetSpeed() / 100.0f) * frameRateMul;

		m_currentAnim->Update(animDelta * deltaTime);
	}
}

void Sprite::Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv)
{
	if(m_currentSheet && m_currentAnim)
	{
		//Set matrix
		renderer.SetMatrix(GetTransform() * cameraInv);

		//Get current anim frame
		int spriteFrame = m_currentAnim->m_trackSpriteFrame.GetValue(m_currentAnim->GetFrame());

		//Bind material
		m_currentSheet->m_frames[spriteFrame].material->Bind(GetTransform(), cameraInv, renderer.GetProjectionMatrix());

		//Draw vertex buffer
		renderer.DrawVertexBuffer(m_currentSheet->m_primitive->GetVertexBuffer(), m_currentSheet->m_primitive->GetIndexBuffer());
	}
}