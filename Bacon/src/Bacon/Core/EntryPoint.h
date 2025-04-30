#pragma once
#include "Bacon/Core/Base.h"
#include "Bacon/Core/Application.h"

#ifdef BC_PLATFORM_WINDOWS

extern Bacon::Application* Bacon::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Bacon::Log::Init();

	BC_PROFILE_BEGIN_SESSION("Startup", "BaconProfile-Startup.json");
	auto app = Bacon::CreateApplication({ argc, argv });
	BC_PROFILE_END_SESSION();

	BC_PROFILE_BEGIN_SESSION("Runtime", "BaconProfile-Runtime.json");
	app->Run();
	BC_PROFILE_END_SESSION();

	BC_PROFILE_BEGIN_SESSION("Shutdown", "BaconProfile-Shutdown.json");
	delete app;
	BC_PROFILE_END_SESSION();
}
#else
	#error Bacon only supports windows!
#endif // BC_PLATFORM_WINDOWS
