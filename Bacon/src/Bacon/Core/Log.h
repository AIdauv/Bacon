#pragma once

#include "Bacon/Core/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Bacon {

	class Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define BC_CORE_TRACE(...)	    ::Bacon::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BC_CORE_INFO(...)	    ::Bacon::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BC_CORE_WARN(...)	    ::Bacon::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BC_CORE_ERROR(...)	    ::Bacon::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BC_CORE_CRITICAL(...)   ::Bacon::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define BC_TRACE(...)           ::Bacon::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BC_INFO(...)            ::Bacon::Log::GetClientLogger()->info(__VA_ARGS__)
#define BC_WARN(...)            ::Bacon::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BC_ERROR(...)           ::Bacon::Log::GetClientLogger()->error(__VA_ARGS__)
#define BC_CRITICAL(...)        ::Bacon::Log::GetClientLogger()->critical(__VA_ARGS__)
