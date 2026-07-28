#pragma once

#include "Paradox/Core/Base.h"

#include <string>
#include <vector>
#include <sstream>
#include <functional>

#ifdef PX_INCLUDE_SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#endif

namespace Paradox {

	class PARADOX_API Log
	{
	public:
		using EventCallbackFn = std::function<void(class Event&)>;

		enum Domain
		{
			Core = 0,
			App
		};

		enum Level
		{
			Trace = 0,
			Info,
			Warn,
			Error,
			Critical
		};

		static void Init();
		static void Shutdown();

#ifdef PX_INCLUDE_SPDLOG
		template<typename... Args>
		void LogMessage(Domain domain, Level level, fmt::format_string<Args...> fmt, Args&&... args)
		{
			LogMessageImpl(domain, level, fmt::format(fmt, std::forward<Args>(args)...));
		}
#endif

		template<typename... Args>
		void FallbackLog(Domain domain, Level level, const char* fmt, Args&&... args)
		{
			std::vector<std::string> argStrings = { ToString(std::forward<Args>(args))... };
			FallbackLogImpl(domain, level, fmt, argStrings);
		}

		void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }
		void ClearEventCallback() { m_EventCallback = nullptr; }

		std::string DomainToString(Log::Domain domain);
		std::string LevelToString(Log::Level level);

		static Log& Get() { return *s_Instance; }

	private:
		template<typename T>
		std::string ToString(T&& value)
		{
			std::ostringstream stream;
			stream << std::forward<T>(value);
			return stream.str();
		}

		void CreateLoggers();
		void FallbackLogImpl(Domain domain, Level level, const char* fmt, const std::vector<std::string>& args);
#ifdef PX_INCLUDE_SPDLOG
		void LogMessageImpl(Domain domain, Level level, const std::string& message);
#endif

	private:
#ifdef PX_INCLUDE_SPDLOG
		Shared<spdlog::logger> m_CoreLogger;
		Shared<spdlog::logger> m_AppLogger;
#endif
		EventCallbackFn m_EventCallback;

		static Log* s_Instance;
	};
}

// Core logging macros
#ifdef PX_INCLUDE_SPDLOG
#define PX_CORE_TRACE(...)		::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Trace, __VA_ARGS__)
#define PX_CORE_INFO(...)		::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Info, __VA_ARGS__)
#define PX_CORE_WARN(...)		::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Warn, __VA_ARGS__)
#define PX_CORE_ERROR(...)		::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Error, __VA_ARGS__)
#define PX_CORE_CRITICAL(...)	::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Critical, __VA_ARGS__)
#else
#define PX_CORE_TRACE(...)		::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Trace, __VA_ARGS__)
#define PX_CORE_INFO(...)		::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Info, __VA_ARGS__)
#define PX_CORE_WARN(...)		::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Warn, __VA_ARGS__)
#define PX_CORE_ERROR(...)		::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Error, __VA_ARGS__)
#define PX_CORE_CRITICAL(...)	::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::Core, ::Paradox::Log::Level::Critical, __VA_ARGS__)
#endif

// App logging macros
#ifdef PX_INCLUDE_SPDLOG
#define PX_TRACE(...)			::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Trace, __VA_ARGS__)
#define PX_INFO(...)			::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Info, __VA_ARGS__)
#define PX_WARN(...)			::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Warn, __VA_ARGS__)
#define PX_ERROR(...)			::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Error, __VA_ARGS__)
#define PX_CRITICAL(...)		::Paradox::Log::Get().LogMessage(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Critical, __VA_ARGS__)
#else
#define PX_TRACE(...)			::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Trace, __VA_ARGS__)
#define PX_INFO(...)			::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Info, __VA_ARGS__)
#define PX_WARN(...)			::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Warn, __VA_ARGS__)
#define PX_ERROR(...)			::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Error, __VA_ARGS__)
#define PX_CRITICAL(...)		::Paradox::Log::Get().FallbackLog(::Paradox::Log::Domain::App, ::Paradox::Log::Level::Critical, __VA_ARGS__)
#endif