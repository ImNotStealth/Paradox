#pragma once

#include "Paradox/Core/Base.h"

#include <imgui.h>

#ifdef PX_INCLUDE_VULKAN
struct VkImageView_T;
struct VkDescriptorSet_T;
#endif

namespace Paradox
{
	class PARADOX_API ImGuiUtils
	{
	public:
		static uint64_t GetImageID(Shared<class Image> image);
		static void Image(Shared<class Image> image, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		static void Image(Reference<class Texture2D> texture, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		
		static bool ImageButton(const char* id, Shared<class Image> image, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1), const ImVec4& bgTint = ImVec4(0, 0, 0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		static bool ImageButton(const char* id, Reference<class Texture2D> texture, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1), const ImVec4& bgTint = ImVec4(0, 0, 0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1));
		static bool IconButton(const char* id, Reference<class Texture2D> texture, const ImVec2& size, const ImVec4& tint = ImVec4(1, 1, 1, 1));

		// Returns a sizeDiff of a 1:1 ratio. Use it in a ImGui::SetCursor to then center the image in a 1:1 ratio (if that makes sense)
		static ImVec2 FitSizeToSquare(uint32_t textureWidth, uint32_t textureHeight, float drawSize);

		static void HelpMarker(const char* message, bool sameLine = true);
		static void Tooltip(const char* message);

		static void EnableInput(bool enabled);
		static void ApplyTheme();

#ifdef PX_INCLUDE_VULKAN
	private:
		static void RemoveImageView(VkImageView_T* view);

	private:
		static std::unordered_map<VkImageView_T*, VkDescriptorSet_T*> s_TextureCache;
		friend class VulkanImage;
#endif
	};
}