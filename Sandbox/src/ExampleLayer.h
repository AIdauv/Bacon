#pragma once

#include "Bacon.h"

class ExampleLayer : public Bacon::Layer
{
public:
    ExampleLayer();
    virtual ~ExampleLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Bacon::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Bacon::Event& e) override;
private:
    Bacon::ShaderLibrary m_ShaderLibrary;
    Bacon::Ref<Bacon::Shader> m_Shader;
    Bacon::Ref<Bacon::VertexArray> m_VertexArray;

    Bacon::Ref<Bacon::Shader> m_FlatColorShader;
    Bacon::Ref<Bacon::VertexArray> m_SquareVA;

    Bacon::Ref<Bacon::Texture2D> m_Texture, m_ChernoLogoTexture;

    Bacon::OrthographicCameraController m_CameraController;
    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};