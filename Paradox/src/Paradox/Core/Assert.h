#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/Log.h"

#include <filesystem>

#ifdef PX_ENABLE_ASSERTS
	#define PX_EXPAND_MACRO(x) x
	#define PX_STRINGIFY_MACRO(x) #x

	#define PX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { PX##type##CRITICAL(msg, __VA_ARGS__); PX_DEBUGBREAK(); } }
	#define PX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) PX_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define PX_INTERNAL_ASSERT_NO_MSG(type, check) PX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", PX_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define PX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define PX_INTERNAL_ASSERT_GET_MACRO(...) PX_EXPAND_MACRO( PX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, PX_INTERNAL_ASSERT_WITH_MSG, PX_INTERNAL_ASSERT_NO_MSG) )

	#define PX_ASSERT(...) PX_EXPAND_MACRO( PX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define PX_CORE_ASSERT(...) PX_EXPAND_MACRO( PX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define PX_ASSERT(...)
	#define PX_CORE_ASSERT(...)
#endif
