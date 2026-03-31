#include "pxpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Paradox {

	Log* Log::s_Instance = nullptr;

	void Log::Init()
	{
		if (s_Instance != nullptr)
			return;

		s_Instance = new Log();
		s_Instance->CreateLoggers();
	}

	void Log::CreateLoggers()
	{
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
	}

	void Log::Shutdown()
	{
		if (s_Instance == nullptr)
			return;

		delete s_Instance;
		s_Instance = nullptr;
	}
}
