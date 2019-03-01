#pragma once

#include <ion/renderer/Renderer.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Material.h>

class Letterbox
{
public:
	Letterbox();
	~Letterbox();

	void Begin();
	void End();

	void Update(float deltaTime);
	void Render(ion::render::Renderer& renderer);

private:
	float m_height;
	float m_speed;
	ion::render::Quad* m_quad;
	ion::render::Material* m_material;
};