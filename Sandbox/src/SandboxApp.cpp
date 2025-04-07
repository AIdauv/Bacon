#include <Bacon.h>
#include <Bacon/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Bacon::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		m_VertexArray = Bacon::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.3f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		Bacon::Ref<Bacon::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Bacon::VertexBuffer::Create(vertices, sizeof(vertices)));


		Bacon::BufferLayout layout = {
			{Bacon::ShaderDataType::Float3, "a_Position"},
			{Bacon::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Bacon::Ref<Bacon::IndexBuffer> indexBuffer;
		indexBuffer.reset(Bacon::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = Bacon::VertexArray::Create();

		float SquareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Bacon::Ref<Bacon::VertexBuffer> squareVB;
		squareVB.reset(Bacon::VertexBuffer::Create(SquareVertices, sizeof(SquareVertices)));
		squareVB->SetLayout({
			{Bacon::ShaderDataType::Float3, "a_Position"},
			{Bacon::ShaderDataType::Float2, "a_TexCoord"}
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Bacon::Ref<Bacon::IndexBuffer> squareIB;
		squareIB.reset(Bacon::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			
			out vec3 v_Position;			
			out vec4 v_Color;			

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;		
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = Bacon::Shader::Create("vertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;			
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;
			
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Bacon::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Bacon::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = Bacon::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Bacon::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Bacon::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}


	void OnUpdate(Bacon::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Bacon::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Bacon::RenderCommand::Clear();

		Bacon::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Bacon::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Bacon::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Bacon::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Bacon::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_ChernoLogoTexture->Bind();
		Bacon::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Traiangle
		//Bacon::Renderer::Submit(m_Shader, m_VertexArray);

		Bacon::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		//auto stats = Bacon::Renderer::GetStats();
		//ImGui::Text("Renderer Stats:");
		//ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		//ImGui::Text("Quad Count: %d", stats.QuadCount);
		//ImGui::Text("Vertex Count: %d", stats.GetTotalVertexCount());
		//ImGui::Text("Index Count: %d", stats.GetTotalIndexCount());
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Bacon::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}
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

class Sandbox : public Bacon::Application
{
public:
	Sandbox()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}
	
	~Sandbox()
	{

	}

private:

};

Bacon::Application* Bacon::CreateApplication()
{
	return new Sandbox();
}