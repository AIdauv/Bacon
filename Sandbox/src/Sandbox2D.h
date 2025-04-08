#pragma once

#include "Bacon.h"

class Sandbox2D : public Bacon::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Bacon::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Bacon::Event& e) override;
private:
	Bacon::OrthographicCameraController m_CameraController;

	// Temp
	Bacon::Ref<Bacon::VertexArray> m_SquareVA;
	Bacon::Ref<Bacon::Shader> m_FlatColorShader;

	Bacon::Ref<Bacon::Texture2D> m_Texture;
	Bacon::Ref<Bacon::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};