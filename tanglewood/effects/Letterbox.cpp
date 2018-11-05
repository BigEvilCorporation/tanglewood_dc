#include "Letterbox.h"
#include "Constants.h"
#include "Globals.h"
#include "Shaders.h"

Letterbox::Letterbox()
{
	m_speed = 0.0f;
	m_height = 0.0f;
	m_quad = new ion::render::Quad(ion::render::Quad::xy, ion::Vector2(Globals::Rendering::windowWidth / 2, Constants::Effects::Letterbox::barHeight / 2));
	m_material = new ion::render::Material();
	m_material->SetDiffuseColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f));

#if defined ION_RENDERER_SHADER
	m_material->SetVertexShader(Assets::Shaders::FlatColoured::vertexShader.Get());
	m_material->SetPixelShader(Assets::Shaders::FlatColoured::pixelShader.Get());
#endif
}

Letterbox::~Letterbox()
{
	delete m_quad;
	delete m_material;
}

void Letterbox::Begin()
{
	m_speed = Constants::Effects::Letterbox::moveSpeed;
}

void Letterbox::End()
{
	m_speed = -Constants::Effects::Letterbox::moveSpeed;
}

void Letterbox::Update(float deltaTime)
{
	if (!ion::maths::IsZero(m_speed) && !ion::maths::IsZero(deltaTime))
	{
		m_height += m_speed * deltaTime;

		if (m_height <= 0.0f)
		{
			m_height = 0.0f;
			m_speed = 0.0f;
		}
		else if (m_height >= Constants::Effects::Letterbox::barHeight)
		{
			m_height = Constants::Effects::Letterbox::barHeight;
			m_speed = 0.0f;
		}
	}
}

void Letterbox::Render(ion::render::Renderer& renderer)
{
	ion::Matrix4 quadMatrix;

	//Top bar
	quadMatrix.SetTranslation(ion::Vector3(Globals::Rendering::windowWidth / 2, m_height - (Constants::Effects::Letterbox::barHeight / 2), 0.0f));
	m_material->Bind(quadMatrix, ion::Matrix4(), renderer.GetProjectionMatrix());
	renderer.DrawVertexBuffer(m_quad->GetVertexBuffer(), m_quad->GetIndexBuffer());
	m_material->Unbind();

	//Bottom bar
	quadMatrix.SetTranslation(ion::Vector3(Globals::Rendering::windowWidth / 2, Globals::Rendering::windowHeight - m_height + (Constants::Effects::Letterbox::barHeight / 2), 0.0f));
	m_material->Bind(quadMatrix, ion::Matrix4(), renderer.GetProjectionMatrix());
	renderer.DrawVertexBuffer(m_quad->GetVertexBuffer(), m_quad->GetIndexBuffer());
	m_material->Unbind();
}