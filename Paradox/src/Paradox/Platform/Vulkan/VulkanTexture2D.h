#pragma once

#include "Paradox/Renderer/Texture.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"

namespace Paradox
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const std::string& debugName, const std::filesystem::path& filePath);
		VulkanTexture2D(const TextureProperties& props, const std::filesystem::path& filePath);
		VulkanTexture2D(const TextureProperties& props, Shared<Image> image);
		~VulkanTexture2D();

		uint32_t GetWidth() const override { return m_Properties.width; }
		uint32_t GetHeight() const override { return m_Properties.height; }
		TextureProperties& GetProperties() override { return m_Properties; }

		inline Shared<Image> GetImage() const { return m_Image; }
		inline VkSampler GetSampler() const { return m_Sampler; }

	private:
		void Create(const std::filesystem::path& filePath);
		void CreateSampler();
		VkFilter GetVulkanFilter(TextureFilter filter);
		VkSamplerAddressMode GetVulkanWrap(TextureWrap wrap);

	private:
		TextureProperties m_Properties;
		Shared<Image> m_Image = nullptr;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};
}