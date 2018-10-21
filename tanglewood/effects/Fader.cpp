#include "Fader.h"
#include "Globals.h"
#include "Shaders.h"

Fader::Fader()
{
	m_fader = 0.0f;
	m_fadeSpeed = 0.0f;
	m_fadeQuad = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2(Globals::Rendering::windowWidth / 2, Globals::Rendering::windowHeight / 2));
	m_fadeMaterial = new ion::render::Material();
	m_fadeMaterial->SetDiffuseColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f));

#if defined ION_RENDERER_SHADER
	m_fadeMaterial->SetVertexShader(Assets::Shaders::FlatColoured::vertexShader.Get());
	m_fadeMaterial->SetPixelShader(Assets::Shaders::FlatColoured::pixelShader.Get());
#endif
}

Fader::~Fader()
{
	delete m_fadeQuad;
	delete m_fadeMaterial;
}

void Fader::ResetToBlack()
{
	m_fadeSpeed = 0.0f;
	m_fader = 0.0f;
}

bool Fader::BeginFade(float speed)
{
	m_fadeSpeed = speed;
	return ((speed < 1.0f && m_fader > 0.0f) || (speed > 1.0f && m_fader < 1.0f));
}

bool Fader::IsFading() const
{
	return m_fadeSpeed != 0.0f;
}

void Fader::Update(float deltaTime)
{
	if (!ion::maths::IsZero(m_fadeSpeed) && !ion::maths::IsZero(deltaTime))
	{
		m_fader += m_fadeSpeed * deltaTime;

		if (m_fader <= 0.0f)
		{
			m_fader = 0.0f;
			m_fadeSpeed = 0.0f;
		}
		else if (m_fader >= 1.0f)
		{
			m_fader = 1.0f;
			m_fadeSpeed = 0.0f;
		}

		m_fadeMaterial->SetDiffuseColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f - m_fader));
	}
}

void Fader::Render(ion::render::Renderer& renderer)
{
	ion::Matrix4 quadMatrix;
	quadMatrix.SetTranslation(ion::Vector3(Globals::Rendering::windowWidth / 2, Globals::Rendering::windowHeight / 2, 0.0f));
	m_fadeMaterial->Bind(quadMatrix, ion::Matrix4(), renderer.GetProjectionMatrix());
	renderer.DrawVertexBuffer(m_fadeQuad->GetVertexBuffer(), m_fadeQuad->GetIndexBuffer());
	m_fadeMaterial->Unbind();
}