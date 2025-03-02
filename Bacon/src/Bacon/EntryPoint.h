#pragma once

#ifdef BC_PLATFORM_WINDOWS

extern Bacon::Application* Bacon::CreateApplication();

int main()
{
	auto app = Bacon::CreateApplication();
	app->Run();
	delete app;
}
#else
	#error Bacon only supports windows!
#endif // BC_PLATFORM_WINDOWS