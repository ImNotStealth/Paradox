#pragma once

#ifdef PX_PLATFORM_WINDOWS
	#ifdef PX_BUILD_DLL
		#define PARADOX_API __declspec(dllexport)
	#else
		#define PARADOX_API __declspec(dllimport)
	#endif
#else
	#error Non-Windows platforms are not supported!
#endif