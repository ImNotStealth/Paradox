#pragma once

#include "Paradox/Core/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Paradox {

	class PARADOX_API Log
	{
	public:
		static void Init();
		static void Shutdown();

		Shared<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		Shared<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

		static Log& Get() { return *s_Instance; }

	private:
		void CreateLoggers();

	private:
		Shared<spdlog::logger> m_CoreLogger;
		Shared<spdlog::logger> m_ClientLogger;
		static Log* s_Instance;
	};
}

// Core logging macros
#define PX_CORE_TRACE(...)		::Paradox::Log::Get().GetCoreLogger()->trace(__VA_ARGS__)
#define PX_CORE_INFO(...)		::Paradox::Log::Get().GetCoreLogger()->info(__VA_ARGS__)
#define PX_CORE_WARN(...)		::Paradox::Log::Get().GetCoreLogger()->warn(__VA_ARGS__)
#define PX_CORE_ERROR(...)		::Paradox::Log::Get().GetCoreLogger()->error(__VA_ARGS__)
#define PX_CORE_CRITICAL(...)	::Paradox::Log::Get().GetCoreLogger()->critical(__VA_ARGS__)

// Client logging macros
#define PX_TRACE(...)			::Paradox::Log::Get().GetClientLogger()->trace(__VA_ARGS__)
#define PX_INFO(...)			::Paradox::Log::Get().GetClientLogger()->info(__VA_ARGS__)
#define PX_WARN(...)			::Paradox::Log::Get().GetClientLogger()->warn(__VA_ARGS__)
#define PX_ERROR(...)			::Paradox::Log::Get().GetClientLogger()->error(__VA_ARGS__)
#define PX_CRITICAL(...)		::Paradox::Log::Get().GetClientLogger()->critical(__VA_ARGS__)