#pragma once

#ifdef BC_PLATFORM_WINDOWS

extern Bacon::Application* Bacon::CreateApplication();

int main()
{
	Bacon::Log::Init();
	BC_CORE_WARN("Initialized Log!");
	int a = 5;
	BC_INFO("Hello! var = {0}", a);

	auto app = Bacon::CreateApplication();
	app->Run();
	delete app;
}
#else
	#error Bacon only supports windows!
#endif // BC_PLATFORM_WINDOWS