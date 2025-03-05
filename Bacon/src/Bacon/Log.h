#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Bacon {
	class BACON_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define BC_CORE_TRACE(...) ::Bacon::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BC_CORE_INFO(...)  ::Bacon::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BC_CORE_WARN(...)  ::Bacon::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BC_CORE_ERROR(...) ::Bacon::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BC_CORE_FATAL(...) ::Bacon::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define BC_TRACE(...)      ::Bacon::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BC_INFO(...)       ::Bacon::Log::GetClientLogger()->info(__VA_ARGS__)
#define BC_WARN(...)       ::Bacon::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BC_ERROR(...)      ::Bacon::Log::GetClientLogger()->error(__VA_ARGS__)
#define BC_FATAL(...)      ::Bacon::Log::GetClientLogger()->fatal(__VA_ARGS__)
