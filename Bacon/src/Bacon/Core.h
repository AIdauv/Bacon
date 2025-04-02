#pragma once

#include <memory>

#ifdef BC_PLATFORM_WINDOWS
	#define FMT_UNICODE 0
	#if HZ_DYNAMIC_LINK
		#ifdef BC_BUILD_DLL
			#define BACON_API __declspec(dllexport)
		#else
			#define BACON_API __declspec(dllimport)
		#endif // BC_BUILD_DLL
	#else
		#define BACON_API
	#endif
#else
	#error Bacon only supports windows!
#endif // BC_PLATFORM_WINDOWS

#ifdef BC_DEBUG
	#define BC_ENABLE_ASSERTS
#endif // BC_DEBUG

#ifdef BC_ENABLE_ASSERTS
	#define BC_ASSERT(x, ...) { if(!(x)) { BC_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define BC_CORE_ASSERT(x, ...) { if(!(x)) { BC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define BC_ASSERT(x, ...)
	#define BC_CORE_ASSERT(x, ...)
#endif // BCENABLE_ASSERTS


#define BIT(x) (1 << x)

#define BC_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Bacon {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}