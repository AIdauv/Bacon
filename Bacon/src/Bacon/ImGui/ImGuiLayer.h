#pragma once

#include "Bacon/Core/Layer.h"

#include "Bacon/Events/ApplicationEvent.h"
#include "Bacon/Events/KeyEvent.h"
#include "Bacon/Events/MouseEvent.h"

namespace Bacon {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}