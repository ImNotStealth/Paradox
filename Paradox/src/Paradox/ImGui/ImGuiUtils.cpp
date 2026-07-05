#include "pxpch.h"
#include "ImGuiUtils.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanTexture2D.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"
#include <backends/imgui_impl_vulkan.h>
#endif
#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLTexture2D.h"
#include "Paradox/Platform/OpenGL/OpenGLImage.h"
#endif

#include <imgui.h>

namespace Paradox
{
	uint64_t ImGuiUtils::GetImageID(Shared<class Image> image)
	{
		GraphicsAPIType api = GraphicsContext::GetGraphicsAPI();
		switch (api)
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
		{
			static std::unordered_map<VkImageView, VkDescriptorSet> s_TextureCache;
			Shared<VulkanImage> vulkanImage = std::static_pointer_cast<VulkanImage>(image);
			VkImageView imageView = vulkanImage->GetImageView();

			auto it = s_TextureCache.find(imageView);
			VkDescriptorSet descriptorSet;
			if (it == s_TextureCache.end())
			{
				descriptorSet = ImGui_ImplVulkan_AddTexture(imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				s_TextureCache.emplace(imageView, descriptorSet);
			}
			else
				descriptorSet = it->second;

			return (uint64_t)descriptorSet;
		}
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
		{
			Shared<OpenGLImage> openglImage = std::static_pointer_cast<OpenGLImage>(image);
			return openglImage->GetHandle();
		}
#endif
		default:
			PX_CORE_ASSERT(false, "ImGuiUtils::Image: Unsupported Graphics API");
			return 0;
		}
	}

	void ImGuiUtils::Image(Shared<class Image> image, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1)
	{
		ImGui::Image(GetImageID(image), size, uv0, uv1);
	}

	void ImGuiUtils::Image(Shared<class Texture2D> texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1)
	{
		ImGuiUtils::Image(texture->GetImage(), size, uv0, uv1);
	}

	void ImGuiUtils::ApplyTheme()
	{
		ImGuiIO& io = ImGui::GetIO();

		float fontSize = 16.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Regular.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Bold.ttf", fontSize);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowTitleAlign = { 0.5, 0.5 };
		style.WindowRounding = 6.f;
		style.TabBorderSize = 1.5f;
		style.FrameBorderSize = 1.5f;
		style.FrameRounding = 4.f;
		style.FramePadding = { 14.f, 6.f };
		style.GrabRounding = 4.f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.34f, 0.34f, 0.34f, 0.78f);
	}
}