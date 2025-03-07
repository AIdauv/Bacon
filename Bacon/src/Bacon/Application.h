#pragma once

#include "Core.h"

#include "Window.h"
#include "Bacon/Layerstack.h"
#include "Bacon/Events/Event.h"

#include "Bacon/Events/ApplicationEvent.h"

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
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};
	
	// To be define in CLIENT
	Application* CreateApplication();
}


