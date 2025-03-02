#pragma once
#include "Core.h"

namespace Bacon {
	class BACON_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be define in CLIENT
	Application* CreateApplication();
}


