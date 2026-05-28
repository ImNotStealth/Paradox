#pragma once

#include "Paradox/Core/Base.h"

#include <string>
#include <vector>
#include <stdio.h>
#include <sstream>

#ifdef PX_INCLUDE_SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#endif

namespace Paradox {

	class PARADOX_API Log
	{
	public:
		static void Init();
		static void Shutdown();

#ifdef PX_INCLUDE_SPDLOG
		Shared<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		Shared<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
#endif

		template<typename... Args>
		void FallbackLog(const char* fmt, Args&&... args)
		{
			std::vector<std::string> argStrings = {
				ToString(std::forward<Args>(args))...
			};

			FallbackLogImpl(fmt, argStrings);
		}

		static Log& Get() { return *s_Instance; }

	private:
		template<typename T>
		std::string ToString(T&& value)
		{
			std::ostringstream stream;
			stream << std::forward<T>(value);
			return stream.str();
		}

		void FallbackLogImpl(const char* fmt, const std::vector<std::string>& args);
		void CreateLoggers();

	private:
#ifdef PX_INCLUDE_SPDLOG
		Shared<spdlog::logger> m_CoreLogger;
		Shared<spdlog::logger> m_ClientLogger;
#endif
		static Log* s_Instance;
	};
}

// Core logging macros
#ifdef PX_INCLUDE_SPDLOG
#define PX_CORE_TRACE(...)		::Paradox::Log::Get().GetCoreLogger()->trace(__VA_ARGS__)
#define PX_CORE_INFO(...)		::Paradox::Log::Get().GetCoreLogger()->info(__VA_ARGS__)
#define PX_CORE_WARN(...)		::Paradox::Log::Get().GetCoreLogger()->warn(__VA_ARGS__)
#define PX_CORE_ERROR(...)		::Paradox::Log::Get().GetCoreLogger()->error(__VA_ARGS__)
#define PX_CORE_CRITICAL(...)	::Paradox::Log::Get().GetCoreLogger()->critical(__VA_ARGS__)
#else
#define PX_CORE_TRACE(...)		::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_CORE_INFO(...)		::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_CORE_WARN(...)		::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_CORE_ERROR(...)		::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_CORE_CRITICAL(...)	::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#endif

// Client logging macros
#ifdef PX_INCLUDE_SPDLOG
#define PX_TRACE(...)			::Paradox::Log::Get().GetClientLogger()->trace(__VA_ARGS__)
#define PX_INFO(...)			::Paradox::Log::Get().GetClientLogger()->info(__VA_ARGS__)
#define PX_WARN(...)			::Paradox::Log::Get().GetClientLogger()->warn(__VA_ARGS__)
#define PX_ERROR(...)			::Paradox::Log::Get().GetClientLogger()->error(__VA_ARGS__)
#define PX_CRITICAL(...)		::Paradox::Log::Get().GetClientLogger()->critical(__VA_ARGS__)
#else
#define PX_TRACE(...)			::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_INFO(...)			::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_WARN(...)			::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_ERROR(...)			::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#define PX_CRITICAL(...)		::Paradox::Log::Get().FallbackLog(__VA_ARGS__)
#endif