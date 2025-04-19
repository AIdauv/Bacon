#pragma once

#include "Bacon.h"

namespace Bacon{

	class EditorLayer : public Bacon::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

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
		Bacon::Ref<Bacon::Framebuffer> m_Framebuffer;

		Bacon::Ref<Bacon::Texture2D> m_CheckerboardTexture;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	};
}

