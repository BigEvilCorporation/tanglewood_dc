#include "Debug.h"
#include <sstream>

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