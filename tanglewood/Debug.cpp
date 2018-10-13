#include "Debug.h"
#include "Constants.h"
#include "framework/PlanePriority.h"

#include <ion/maths/Geometry.h>

#include <sstream>

namespace Debug
{
	namespace Draw
	{
		void DrawLineQuad(const ion::Vector2& topLeft, ion::Vector2& bottomRight, const ion::Colour& colour, ion::render::Renderer& renderer, const ion::Matrix4& viewMtx)
		{
			ion::Vector2 scale = bottomRight - topLeft;
			ion::Vector2 centre = topLeft + (scale / 2.0f);
			ion::Matrix4 worldMtx;
			worldMtx.SetTranslation(ion::Vector3(centre.x, centre.y, Constants::Rendering::planePriorities[(int)PlanePriority::Debug]));
			worldMtx.SetScale(ion::Vector3(scale.x, scale.y, 1.0f));

			Assets::Materials::lineDraw->SetDiffuseColour(colour);
			Assets::Materials::lineDraw->Bind(worldMtx, viewMtx, renderer.GetProjectionMatrix());
			renderer.DrawVertexBuffer(Assets::Primitives::lineQuad->GetVertexBuffer());
			Assets::Materials::lineDraw->Unbind();
		}
	}

	namespace Assets
	{
		void LoadAll(ion::io::ResourceManager& resourceManager)
		{
#if defined ION_RENDERER_SHADER
			Shaders::pixelShaderLineDraw = resourceManager.GetResource<ion::render::Shader>("flatcoloured_p");
			Shaders::vertexShaderLineDraw = resourceManager.GetResource<ion::render::Shader>("flatcoloured_v");
#endif
		}

		void Init()
		{
			Primitives::quad = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2(0.5f, 0.5f));
			Primitives::lineQuad = new ion::render::LineQuad(ion::render::LineQuad::xy, ion::Vector2(0.5f, 0.5f));
			Materials::lineDraw = new ion::render::Material();

#if defined ION_RENDERER_SHADER
			Materials::lineDraw->SetPixelShader(Shaders::pixelShaderLineDraw.Get());
			Materials::lineDraw->SetVertexShader(Shaders::vertexShaderLineDraw.Get());
#endif
		}

		void Shutdown()
		{
			delete Primitives::quad;
			delete Primitives::lineQuad;
			delete Materials::lineDraw;

#if defined ION_RENDERER_SHADER
			Shaders::pixelShaderLineDraw.Clear();
			Shaders::vertexShaderLineDraw.Clear();
#endif
		}

		namespace Primitives
		{
			ion::render::Quad* quad = nullptr;
			ion::render::LineQuad* lineQuad = nullptr;
		}

		namespace Materials
		{
			ion::render::Material* lineDraw = nullptr;
		}

		namespace Shaders
		{
#if defined ION_RENDERER_SHADER
			ion::io::ResourceHandle<ion::render::Shader> vertexShaderLineDraw;
			ion::io::ResourceHandle<ion::render::Shader> pixelShaderLineDraw;
#endif
		}
	}
}

DebugUI::DebugUI(ion::gui::GUI& gui, const ion::Vector2i& position, const ion::Vector2i& size)
	: ion::gui::Window("Debug", position, size)
	, m_gui(gui)
{
	AllowResize(true);
	AllowMove(true);
}

DebugUI::~DebugUI()
{
	for (int i = 0; i < m_objUIs.size(); i++)
	{
		delete m_objUIs[i].second;
	}
}

void DebugUI::Update(float deltaTime)
{
	for (int i = 0; i < m_objUIs.size(); i++)
	{
		UpdateObjUI(*m_objUIs[i].first, *m_objUIs[i].second);
	}

	Window::Update(deltaTime);
}

void DebugUI::AddWatchObj(const SpriteObj& obj)
{
	m_objUIs.push_back(std::make_pair(&obj, new ObjUI()));

	ObjUI& objUI = *m_objUIs.back().second;

	AddWidget(objUI.name);
	AddWidget(objUI.posX);
	AddWidget(objUI.posY);
	AddWidget(objUI.animTime);
	AddWidget(objUI.animSpeed);
}

void DebugUI::UpdateObjUI(const SpriteObj& obj, ObjUI& objUI)
{
	std::stringstream stream;

	stream << "Name: " << obj.m_name;
	objUI.name.SetText(stream.str());

	stream.str("");
	stream << "Pos X: " << obj.m_worldPos.x;
	objUI.posX.SetText(stream.str());

	stream.str("");
	stream << "Pos Y: " << obj.m_worldPos.y;
	objUI.posY.SetText(stream.str());

	stream.str("");
	stream << "Anim Time: " << (obj.GetCurrentAnimation() ? obj.GetCurrentAnimation()->GetFrame() : 0.0f);
	objUI.animTime.SetText(stream.str());

	stream.str("");
	stream << "Anim Speed: " << (obj.GetCurrentAnimation() ? obj.GetCurrentAnimation()->GetPlaybackSpeed() : 0.0f);
	objUI.animSpeed.SetText(stream.str());
}