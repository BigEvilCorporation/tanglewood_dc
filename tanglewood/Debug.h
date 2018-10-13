#pragma once

#include <ion/gui/GUI.h>
#include <ion/gui/Window.h>
#include <ion/gui/Button.h>
#include <ion/gui/CheckBox.h>
#include <ion/gui/ComboBox.h>
#include <ion/gui/TextBox.h>

#include <ion/renderer/Colour.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Material.h>
#include <ion/renderer/Shader.h>
#include <ion/renderer/Camera.h>
#include <ion/io/ResourceManager.h>

#include <vector>
#include <tuple>

#include "framework/SpriteObj.h"

namespace Debug
{
	namespace Draw
	{
		//Draws a line quad, OpenGL coordinate system
		void DrawLineQuad(const ion::Vector2& topLeft, ion::Vector2& bottomRight, const ion::Colour& colour, ion::render::Renderer& renderer, const ion::Matrix4& viewMtx);
	}

	namespace Assets
	{
		void LoadAll(ion::io::ResourceManager& resourceManager);
		void Init();
		void Shutdown();

		namespace Primitives
		{
			extern ion::render::Quad* quad;
			extern ion::render::LineQuad* lineQuad;
		}

		namespace Materials
		{
			extern ion::render::Material* lineDraw;
		}

		namespace Shaders
		{
#if defined ION_RENDERER_SHADER
			extern ion::io::ResourceHandle<ion::render::Shader> vertexShaderLineDraw;
			extern ion::io::ResourceHandle<ion::render::Shader> pixelShaderLineDraw;
#endif
		}
	}
}

class DebugUI : public ion::gui::Window
{
public:
	DebugUI(ion::gui::GUI& gui, const ion::Vector2i& position, const ion::Vector2i& size);
	~DebugUI();

	virtual void Update(float deltaTime);

	void AddWatchObj(const SpriteObj& obj);

private:
	struct ObjUI
	{
		ion::gui::TextBox name;
		ion::gui::TextBox posX;
		ion::gui::TextBox posY;
		ion::gui::TextBox animTime;
		ion::gui::TextBox animSpeed;
	};

	void UpdateObjUI(const SpriteObj& obj, ObjUI& objUI);

	ion::gui::GUI& m_gui;
	
	std::vector<std::pair<const SpriteObj*, ObjUI*>> m_objUIs;
};
