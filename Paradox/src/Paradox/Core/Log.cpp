#include "pxpch.h"
#include "Log.h"

#include "Paradox/Events/ApplicationEvents.h"

#ifdef PX_INCLUDE_SPDLOG
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#endif

namespace Paradox {

	Log* Log::s_Instance = nullptr;

	void Log::Init()
	{
		if (s_Instance != nullptr)
			return;

		s_Instance = new Log();
		s_Instance->CreateLoggers();
	}

	std::string Log::DomainToString(Domain domain)
	{
		switch (domain)
		{
		case Domain::Core: return "Paradox";
		case Domain::App: return "App";
		}

		PX_CORE_ASSERT(false, "Unknown Log::Domain");
		return "Unknown";
	}

	std::string Log::LevelToString(Level level)
	{
		switch (level)
		{
		case Level::Trace:		return "Trace";
		case Level::Info:		return "Info";
		case Level::Warn:		return "Warn";
		case Level::Error:		return "Error";
		case Level::Critical:	return "Critical";
		}

		PX_CORE_ASSERT(false, "Unknown Log::Level");
		return "Unknown";
	}

	void Log::CreateLoggers()
	{
#ifdef PX_INCLUDE_SPDLOG
		auto colorSink = CreateShared<spdlog::sinks::stdout_color_sink_mt>();
		colorSink->set_pattern("%^[%T.%e] %n: %v%$");
#ifdef PX_PLATFORM_WINDOWS
		colorSink->set_color(spdlog::level::trace, 0x8);
		colorSink->set_color(spdlog::level::info, 0x7);
#else
		colorSink->set_color(spdlog::level::trace, "\033[90m");
		colorSink->set_color(spdlog::level::info, "\033[0m");
#endif

		auto fileSink = CreateShared<spdlog::sinks::basic_file_sink_mt>("Paradox.log", true);
		fileSink->set_pattern("[%T.%e] [%l] %n: %v");

		std::vector<spdlog::sink_ptr> sinks;
		sinks.emplace_back(colorSink);
		sinks.emplace_back(fileSink);

		m_CoreLogger = CreateShared<spdlog::logger>("Paradox", std::begin(sinks), std::end(sinks));
		spdlog::register_logger(m_CoreLogger);
		m_CoreLogger->set_level(spdlog::level::trace);
		m_CoreLogger->flush_on(spdlog::level::trace);

		m_AppLogger = CreateShared<spdlog::logger>("App", std::begin(sinks), std::end(sinks));
		spdlog::register_logger(m_AppLogger);
		m_AppLogger->set_level(spdlog::level::trace);
		m_AppLogger->flush_on(spdlog::level::trace);
#endif
	}

	// This function acts as a replacement and manually parses spdlog's "{x}" formatting.
	// It was done to be able to compile to platforms that don't support spdlog (ex: PSVita)
	void Log::FallbackLogImpl(Domain domain, Level level, const char* fmt, const std::vector<std::string>& args)
	{
		std::ostringstream result;
		while (*fmt)
		{
			if (fmt[0] == '{')
			{
				fmt++;
				size_t index = 0;
				while (*fmt >= '0' && *fmt <= '9')
				{
					index = index * 10 + (*fmt - '0');
					fmt++;
				}
				if (*fmt == '}')
				{
					if (index < args.size())
						result << args[index];
					fmt++;
					continue;
				}
			}
			result << *fmt++;
		}

		std::string message = "[" + LevelToString(level) + "] " + DomainToString(domain) + ": " + result.str();
		printf("%s\n", message.c_str());

		static std::ofstream logFile("Paradox.log", std::ios::app);
		if (logFile.is_open())
		{
			logFile << message << '\n';
			logFile.flush();
		}

		if (m_EventCallback)
		{
			ConsoleLogEvent event(domain, level, result.str());
			m_EventCallback(event);
		}
	}

#ifdef PX_INCLUDE_SPDLOG
	void Log::LogMessageImpl(Domain domain, Level level, const std::string& message)
	{
		Shared<spdlog::logger> logger = domain == Domain::Core ? m_CoreLogger : m_AppLogger;

		switch (level)
		{
		case Level::Trace:
			logger->trace(message);
			break;
		case Level::Info:
			logger->info(message);
			break;
		case Level::Warn:
			logger->warn(message);
			break;
		case Level::Error:
			logger->error(message);
			break;
		case Level::Critical:
		default:
			logger->critical(message);
			break;
		}

		if (m_EventCallback)
		{
			ConsoleLogEvent event(domain, level, message);
			m_EventCallback(event);
		}
	}
#endif

	void Log::Shutdown()
	{
		if (s_Instance == nullptr)
			return;

		delete s_Instance;
		s_Instance = nullptr;
	}
}
