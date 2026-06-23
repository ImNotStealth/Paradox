#pragma once

#include "Paradox/Renderer/Texture.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"

namespace Paradox
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& debugName, const std::filesystem::path& filePath);
		~VulkanTexture();

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		inline VkImageView GetImageView() const { return m_ImageView; }
		inline VkSampler GetSampler() const { return m_Sampler; }

	private:
		std::string m_DebugName;
		uint32_t m_Width, m_Height;
		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};
}