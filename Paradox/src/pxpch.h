#pragma once

#include "Paradox/Core/PlatformDetection.h"

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>

//#include "Paradox/Core/Log.h"
#include "Paradox/Core/Version.h"

#ifdef PX_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
#endif
