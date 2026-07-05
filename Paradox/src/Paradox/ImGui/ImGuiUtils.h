#pragma once

#include "Paradox/Core/Base.h"

#include <imgui.h>

namespace Paradox
{
	class PARADOX_API ImGuiUtils
	{
	public:
		static uint64_t GetImageID(Shared<class Image> image);
		static void Image(Shared<class Image> image, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		static void Image(Shared<class Texture2D> texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		static void ApplyTheme();
	};
}