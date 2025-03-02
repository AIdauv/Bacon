#pragma once

#ifdef BC_PLATFORM_WINDOWS
	#ifdef BC_BUILD_DLL
		#define BACON_API __declspec(dllexport)
	#else
		#define BACON_API __declspec(dllimport)
	#endif // BC_BUILD_DLL
#else
	#error Bacon only supports windows!
#endif // BC_PLATFORM_WINDOWS
