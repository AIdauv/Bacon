#pragma once

#include "Core.h"

#include "Window.h"
#include "Bacon/Layerstack.h"
#include "Bacon/Events/Event.h"

#include "Bacon/Events/ApplicationEvent.h"

#include "Bacon/ImGui/ImGuiLayer.h"

#include "Bacon/Renderer/Shader.h"
#include "Renderer/Buffer.h"

namespace Bacon {
	class BACON_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		unsigned int m_VertexArray;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;	
	private:
		static Application* s_Instance;
	};
	
	// To be define in CLIENT
	Application* CreateApplication();
}


