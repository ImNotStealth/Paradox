#pragma once

#include "Paradox/Core/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Paradox {

	class PARADOX_API Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }	

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core logging macros
#define PX_CORE_TRACE(...)		::Paradox::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PX_CORE_INFO(...)		::Paradox::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PX_CORE_WARN(...)		::Paradox::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PX_CORE_ERROR(...)		::Paradox::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PX_CORE_CRITICAL(...)	::Paradox::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client logging macros
#define PX_TRACE(...)			::Paradox::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PX_INFO(...)			::Paradox::Log::GetClientLogger()->info(__VA_ARGS__)
#define PX_WARN(...)			::Paradox::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PX_ERROR(...)			::Paradox::Log::GetClientLogger()->error(__VA_ARGS__)
#define PX_CRITICAL(...)		::Paradox::Log::GetClientLogger()->critical(__VA_ARGS__)