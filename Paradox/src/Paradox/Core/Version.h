#pragma once

#define PX_VERSION "0.1.0"

#if defined(PX_DEBUG)
	#define PX_BUILD_CONFIG_NAME "Debug"
#elif defined(PX_RELEASE)
	#define PX_BUILD_CONFIG_NAME "Release"
#elif defined(PX_DIST)
	#define PX_BUILD_CONFIG_NAME "Dist"
#else
	#error Unknown build config.
#endif