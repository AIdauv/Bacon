#pragma once

#include "Bacon/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

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

template <glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> : fmt::formatter<std::string>
{
	auto format(const glm::vec<L, T, Q>& vec, format_context& ctx) const
	{
		return fmt::formatter<std::string>::format(glm::to_string(vec), ctx);
	}
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>> : fmt::formatter<std::string>
{
	auto format(const glm::mat<C, R, T, Q>& mat, format_context& ctx) const
	{
		return fmt::formatter<std::string>::format(glm::to_string(mat), ctx);
	}
};

template <typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> : fmt::formatter<std::string>
{
	auto format(const glm::qua<T, Q>& quat, format_context& ctx) const
	{
		return fmt::formatter<std::string>::format(glm::to_string(quat), ctx);
	}
};

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
