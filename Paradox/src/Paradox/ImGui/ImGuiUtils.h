#pragma once

#include "Paradox/Core/Base.h"

#include <imgui.h>

namespace Paradox
{
	class PARADOX_API ImGuiUtils
	{
	public:
		static uint64_t GetImageID(Shared<class Image> image);
		static void Image(Shared<class Image> image, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		static void Image(Shared<class Texture2D> texture, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		
		// Returns a sizeDiff of a 1:1 ratio. Use it in a ImGui::SetCursor to then center the image in a 1:1 ratio (if that makes sense)
		static ImVec2 FitSizeToSquare(uint32_t textureWidth, uint32_t textureHeight, float drawSize);

		static void HelpMarker(const char* message, bool sameLine = true);

		static void ApplyTheme();
	};
}