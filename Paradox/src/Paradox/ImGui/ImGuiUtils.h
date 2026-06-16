#pragma once

#include "Paradox/Core/Base.h"

#include <imgui.h>

namespace Paradox
{
	class PARADOX_API ImGuiUtils
	{
		public:
			static void Image(uint64_t imageId, ImVec2 size);
	};
}