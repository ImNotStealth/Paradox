#include "pxpch.h"
#include "Log.h"

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

	// This function acts as a replacement and manually parses spdlog's "{x}" formatting.
	// It was done to be able to compile to platforms that don't support spdlog (ex: PSVita)
	void Log::FallbackLogImpl(const char* fmt, const std::vector<std::string>& args)
	{
		std::ostringstream result;

		size_t autoIndex = 0;

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

		printf("%s\n", result.str().c_str());
	}

	void Log::CreateLoggers()
	{
#ifdef PX_INCLUDE_SPDLOG
		std::vector<spdlog::sink_ptr> sinks;
		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Paradox.log", true));

		sinks[0]->set_pattern("%^[%T.%e] %n: %v%$");
		sinks[1]->set_pattern("[%T.%e] [%l] %n: %v");

		m_CoreLogger = CreateShared<spdlog::logger>("Paradox", std::begin(sinks), std::end(sinks));
		spdlog::register_logger(m_CoreLogger);
		m_CoreLogger->set_level(spdlog::level::trace);
		m_CoreLogger->flush_on(spdlog::level::trace);

		m_ClientLogger = CreateShared<spdlog::logger>("App", std::begin(sinks), std::end(sinks));
		spdlog::register_logger(m_ClientLogger);
		m_ClientLogger->set_level(spdlog::level::trace);
		m_ClientLogger->flush_on(spdlog::level::trace);
#endif
	}

	void Log::Shutdown()
	{
		if (s_Instance == nullptr)
			return;

		delete s_Instance;
		s_Instance = nullptr;
	}
}
