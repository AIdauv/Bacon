#pragma once

#include "Core.h"

#include "Window.h"
#include "Bacon/Core/Layerstack.h"
#include "Bacon/Events/Event.h"

#include "Bacon/Events/ApplicationEvent.h"

#include "Bacon/Core/Timestep.h"

#include "Bacon/ImGui/ImGuiLayer.h"


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
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};
	
	// To be define in CLIENT
	Application* CreateApplication();
}


