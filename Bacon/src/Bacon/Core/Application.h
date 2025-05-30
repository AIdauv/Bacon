#pragma once

#include "Bacon/Core/Base.h"

#include "Bacon/Core/Window.h"
#include "Bacon/Core/Layerstack.h"
#include "Bacon/Events/Event.h"
#include "Bacon/Events/ApplicationEvent.h"

#include "Bacon/Core/Timestep.h"

#include "Bacon/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Bacon {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			BC_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Bacon Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		void Close();
		
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		
		inline static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};
	
	// To be define in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}


