#include "Tanglewood.h"

#include <ion/core/debug/Debug.h>

const char* Tanglewood::s_spriteDataFile = "cd/sprites.bee";

//TODO: Move
#include <ion/core/memory/Memory.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/beehive/SpriteSheet.h>
#include <ion/beehive/SpriteAnimation.h>

class SpriteSheetRenderResources
{
public:
	SpriteSheetRenderResources() { m_primitive = NULL; }

	void Load(const SpriteSheet& spriteSheet);

	struct Frame
	{
		ion::render::Texture* texture;
		ion::render::Material* material;
	};

	ion::render::Chessboard* m_primitive;
	std::vector<Frame> m_frames;
};

SpriteSheetRenderResources spriteRenderer;
SpriteAnimation* animNymnRun = NULL;

Tanglewood::Tanglewood()
	: Application("Tanglewood")
{
	m_levelData = NULL;
	m_spriteData = NULL;
}

Tanglewood::~Tanglewood()
{

}

bool Tanglewood::Initialise()
{
	//Create window/renderer/camera/viewport
	m_window = ion::render::Window::Create("Tanglewood", s_defaultWindowWidth, s_defaultWindowHeight, false);
	m_renderer = ion::render::Renderer::Create(m_window->GetDeviceContext());
	m_camera = new ion::render::Camera();
	m_viewport = new ion::render::Viewport(s_defaultWindowWidth, s_defaultWindowHeight, ion::render::Viewport::eOrtho2DAbsolute);

	//Set initial camera position
	m_camera->SetPosition(ion::Vector3(-(float)s_defaultWindowWidth / 2.0f, -(float)s_defaultWindowHeight / 2.0f, -0.1f));

	//Load sprite data from Beehive project file
	m_spriteData = new Project(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(!m_spriteData->Load(s_spriteDataFile))
	{
		ion::debug::error << "Error loading sprite data " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	//Create sprite resources
	Actor* actorNymn = m_spriteData->FindActor("nymn");
	if(!actorNymn)
	{
		ion::debug::error << "Could not find actor Nymn in " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	SpriteSheet* spriteSheetNymnRun = actorNymn->FindSpriteSheet("run");
	if(!spriteSheetNymnRun)
	{
		ion::debug::error << "Could not find sprite sheet Nymn Run in " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	/*SpriteAnimation**/ animNymnRun = spriteSheetNymnRun->FindAnimation("run");
	if(!animNymnRun)
	{
		ion::debug::error << "Could not find anim Nymn Run in " << s_spriteDataFile << ion::debug::end;
		return false;
	}

	animNymnRun->SetState(ion::render::Animation::ePlaying);
	animNymnRun->SetPlaybackSpeed(animNymnRun->GetSpeed());

	//Create render resources for sprite sheet
	spriteRenderer.Load(*spriteSheetNymnRun);

	//Load first level
	return LoadLevel("cd/lvl1.bee");
}

void Tanglewood::Shutdown()
{
	if(m_levelData)
	{
		delete m_levelData;
	}

	if(m_spriteData)
	{
		delete m_spriteData;
	}

	if(m_viewport)
	{
		delete m_viewport;
	}

	if(m_camera)
	{
		delete m_camera;
	}

	if(m_renderer)
	{
		delete m_renderer;
	}

	if(m_window)
	{
		delete m_window;
	}
}

bool Tanglewood::Update(float deltaTime)
{
	animNymnRun->Update(deltaTime);

	return m_window->Update();
}

void Tanglewood::Render()
{
	m_renderer->BeginFrame(*m_viewport, m_window->GetDeviceContext());
	m_renderer->ClearColour();
	m_renderer->ClearDepth();

#if defined ION_PLATFORM_DREAMCAST
	//TEMP
	m_renderer->SetFaceCulling(ion::render::Renderer::eNoCull);
#endif

	//Draw sprites
	ion::Matrix4 cameraInv = m_camera->GetTransform().GetInverse();
	m_renderer->SetMatrix(cameraInv);
	int spriteFrame = animNymnRun->m_trackSpriteFrame.GetValue(animNymnRun->GetFrame());
	spriteRenderer.m_frames[spriteFrame].material->Bind(ion::Matrix4(), cameraInv, m_renderer->GetProjectionMatrix());
	m_renderer->DrawVertexBuffer(spriteRenderer.m_primitive->GetVertexBuffer(), spriteRenderer.m_primitive->GetIndexBuffer());

	m_renderer->SwapBuffers();
	m_renderer->EndFrame();
}

bool Tanglewood::LoadLevel(const std::string& name)
{
	if(m_levelData)
	{
		delete m_levelData;
	}

	//Load level data from Beehive project file
	m_levelData = new Project(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(!m_levelData->Load(name))
	{
		ion::debug::error << "Error loading level data " << name << ion::debug::end;
		return false;
	}

	return true;
}

void SpriteSheetRenderResources::Load(const SpriteSheet& spriteSheet)
{
	const int tileWidth = 8;
	const int tileHeight = 8;

	m_primitive = new ion::render::Chessboard(ion::render::Chessboard::xy, ion::Vector2((float)spriteSheet.GetWidthTiles() * (tileWidth / 2.0f), (float)spriteSheet.GetHeightTiles() * (tileHeight / 2.0f)), spriteSheet.GetWidthTiles(), spriteSheet.GetHeightTiles(), true);

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
		Frame renderFrame;

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

				float top = textureBottomLeft.y + (1.0f / (float)heightTiles);
				float left = textureBottomLeft.x;
				float bottom = textureBottomLeft.y;
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

				m_primitive->SetTexCoords((tileY * widthTiles) + tileX, coords);
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
		m_frames.push_back(renderFrame);

		delete data;
	}
}