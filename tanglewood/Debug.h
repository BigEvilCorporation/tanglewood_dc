#pragma once

#include <ion/gui/GUI.h>
#include <ion/gui/Window.h>
#include <ion/gui/Button.h>
#include <ion/gui/CheckBox.h>
#include <ion/gui/ComboBox.h>
#include <ion/gui/TextBox.h>

#include <vector>
#include <tuple>

#include "framework/SpriteObj.h"

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
