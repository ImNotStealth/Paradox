#pragma once

#include "Paradox/Renderer/Texture.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"

namespace Paradox
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const std::string& debugName, const std::filesystem::path& filePath);
		VulkanTexture2D(const TextureProperties& props, const std::filesystem::path& filePath);
		~VulkanTexture2D();

		uint32_t GetWidth() const override { return m_Properties.width; }
		uint32_t GetHeight() const override { return m_Properties.height; }
		TextureProperties& GetProperties() override { return m_Properties; }

		inline VkImageView GetImageView() const { return m_ImageView; }
		inline VkSampler GetSampler() const { return m_Sampler; }

	private:
		void Create(const std::filesystem::path& filePath);
		VkFormat GetVulkanFormat(TextureFormat format);
		VkFilter GetVulkanFilter(TextureFilter filter);
		VkSamplerAddressMode GetVulkanWrap(TextureWrap wrap);

	private:
		TextureProperties m_Properties;
		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};
}