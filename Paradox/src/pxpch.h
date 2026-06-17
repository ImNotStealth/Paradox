#pragma once

#include "Paradox/Core/PlatformDetection.h"

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

#include "Paradox/Core/Version.h"

#ifdef PX_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
#endif
