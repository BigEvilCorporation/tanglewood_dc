
#if defined ION_PLATFORM_DREAMCAST
#include <kos.h>
#endif

#if defined ION_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#if defined ION_RENDER_SUPPORTS_GLUT
#include <GL/glut.h>
#endif

#include <ion/core/debug/Debug.h>
#include <ion/core/memory/Memory.h>
#include <ion/io/File.h>
#include <ion/renderer/Renderer.h>
#include <ion/renderer/Camera.h>
#include <ion/renderer/Viewport.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/CoordSys.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/beehive/Project.h>
#include <ion/beehive/SpriteSheet.h>

#if defined ION_PLATFORM_WINDOWS
#include <ion/renderer/Window.h>
#endif

#include <iostream>
#include <sstream>
#include <math.h>

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

static GLfloat rtri;    // Rotation angle for the triangle 
static GLfloat rquad;   // Rotation angle for the quad 

void draw_gl()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(rtri, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -7.0f);
	glRotatef(rquad, 1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	rtri += 0.2f;
	rquad -= 0.15f;
}

int main(int argc, char* argv[])
{
	//maple_device_t *cont;
	//cont_state_t *state;

	// Get basic stuff initialized 
	//glKosInit();

	ion::render::DeviceContext deviceContext;

#if defined ION_PLATFORM_WINDOWS
	ion::render::Window* window = ion::render::Window::Create("Hello world", 640, 480, false);
	deviceContext = window->GetDeviceContext();
#endif

	ion::render::Renderer* renderer = ion::render::Renderer::Create(deviceContext);
	ion::render::Camera* camera = new ion::render::Camera();
	ion::render::Viewport* viewport = new ion::render::Viewport(640, 480, ion::render::Viewport::ePerspective3D);
	ion::render::Primitive* cubePrimitive = new ion::render::Box(ion::Vector3(1.0f, 1.0f, 1.0f));

	camera->SetPosition(ion::Vector3(0.0f, 1.0f, 70.0f));

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//
	//glShadeModel(GL_SMOOTH);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	//float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	//float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float position[] = { 200.0f, 300.0f, 100.0f, 0.0f };
	//
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, position);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHTING);
	//
	//float mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
	//float mat_diffuse[] = { 0.07568f, 0.61424f, 0.07568f, 0.55f };
	//float mat_specular[] = { 0.633f, 0.727811f, 0.633f, 0.55f };
	//float shine = 76.8f;
	//
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialf(GL_FRONT, GL_SHININESS, 27.89743616f);

	viewport->SetClearColour(ion::Colour(0.0f, 0.0f, 0.3f, 1.0f));

	ion::Matrix4 objectMtx;
	float rotation = 0.0f;

	//ion::io::File testFile;
	//if(testFile.Open("/cd/textfile.txt", ion::io::File::eOpenRead))

	Project projectLvl(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	Project projectSprites(PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive]);
	if(projectLvl.Load("cd/lvl1.bee") && projectSprites.Load("cd/sprites.bee"))
	{
		printf("Loaded %i sprite sheet actors\n", projectSprites.GetActorCount());

		SpriteSheetRenderResources spriteRenderer;
		spriteRenderer.Load(*projectSprites.FindActor("nymn")->FindSpriteSheet("idle"));

#if defined ION_PLATFORM_WINDOWS
		while(window->Update())
#else
		while(1)
#endif
		{
			//rotation += 1.0f;

			ion::debug::PrintMemoryUsage();

			renderer->BeginFrame(*viewport, deviceContext);
			renderer->ClearColour();
			renderer->ClearDepth();
			renderer->SetFaceCulling(ion::render::Renderer::eNoCull);
			
			//draw_gl();
			
			ion::Matrix4 objRotMtx ;
			objRotMtx.SetRotation(rotation, ion::Vector3(0.0f, 1.0f, 0.0f));
			
			ion::Matrix4 worldViewMtx = objRotMtx * camera->GetTransform().GetInverse();
			
			renderer->SetMatrix(worldViewMtx);

			spriteRenderer.m_frames[0].material->Bind(objRotMtx, camera->GetTransform().GetInverse(), renderer->GetProjectionMatrix());
			
			//renderer->DrawVertexBuffer(cubePrimitive->GetVertexBuffer(), cubePrimitive->GetIndexBuffer());
			renderer->DrawVertexBuffer(spriteRenderer.m_primitive->GetVertexBuffer(), spriteRenderer.m_primitive->GetIndexBuffer());
			
			renderer->SwapBuffers();
			renderer->EndFrame();

			//cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
			//
			//// Check key status 
			//state = (cont_state_t *)maple_dev_status(cont);
			//
			//if(!state) {
			//	printf("Error reading controller\n");
			//	break;
			//}
			//
			//if(state->buttons & CONT_START)
			//	break;

			// Draw the "scene" 
			//draw_gl();

			// Finish the frame 
			//glutSwapBuffers();
		}
	}

	return 0;
}

void SpriteSheetRenderResources::Load(const SpriteSheet& spriteSheet)
{
	const int tileWidth = 8; // project->GetPlatformConfig().tileWidth;
	const int tileHeight = 8; // project->GetPlatformConfig().tileHeight;

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
		// renderFrame.material->SetVertexShader(vertexShader);
		// renderFrame.material->SetPixelShader(pixelshader);

		//Insert frame
		m_frames.push_back(renderFrame);

		delete data;
	}
}