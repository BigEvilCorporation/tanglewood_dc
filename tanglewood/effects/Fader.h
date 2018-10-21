#pragma once

#include <ion/renderer/Renderer.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Material.h>

class Fader
{
public:
	Fader();
	~Fader();

	void ResetToBlack();
	bool BeginFade(float speed);
	bool IsFading() const;

	void Update(float deltaTime);
	void Render(ion::render::Renderer& renderer);

private:
	float m_fader;
	float m_fadeSpeed;
	ion::render::Quad* m_fadeQuad;
	ion::render::Material* m_fadeMaterial;
};