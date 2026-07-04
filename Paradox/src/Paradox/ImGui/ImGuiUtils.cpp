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
}