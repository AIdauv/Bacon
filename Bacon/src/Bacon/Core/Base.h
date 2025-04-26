#pragma once

#include <memory>

#include "Bacon/Core/PlatformDetection.h"

#ifdef BC_DEBUG
	#if defined(BC_PLATFORM_WINDOWS)
		#define BC_DEBUGBREAK() __debugbreak()
	#elif defined(BC_PLATFORM_LINUX)
		#include <signal.h>
		#define BC_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define BC_ENABLE_ASSERTS
#else
	#define BC_DEBUGBREAK()
#endif

#define BC_EXPAND_MACRO(x) x
#define BC_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define BC_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Bacon {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Bacon/Core/Log.h"
#include "Bacon/Core/Assert.h"
