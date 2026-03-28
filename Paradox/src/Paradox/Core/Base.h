#pragma once

#include <memory>

#include "Paradox/Core/PlatformDetection.h"

#ifdef PX_PLATFORM_WINDOWS
	#ifdef PX_BUILD_DLL
		#define PARADOX_API __declspec(dllexport)
	#else
		#define PARADOX_API __declspec(dllimport)
	#endif

	#define BIT(x) (1 << x)
#else
	#error Non-Windows platforms are not supported!
#endif

#ifdef PX_DEBUG
	#if defined(PX_PLATFORM_WINDOWS)
		#define PX_DEBUGBREAK() __debugbreak()
	#elif defined(PX_PLATFORM_LINUX)
		#include <signal.h>
		#define PX_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak."
	#endif
	#define PX_ENABLE_ASSERTS
#else
	#define PX_DEBUGBREAK()
#endif

namespace Paradox
{
	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename... Args>
	constexpr Unique<T> CreateUnique(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Shared = std::shared_ptr<T>;
	template<typename T, typename... Args>
	constexpr Shared<T> CreateShared(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}


#include "Paradox/Core/Assert.h"