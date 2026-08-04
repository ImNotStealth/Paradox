#pragma once

#include "Paradox/Renderer/Image.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"

namespace Paradox
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage(const ImageProperties& props);
		~VulkanImage();

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetWidth() override { return m_Properties.width; }
		uint32_t GetHeight() override { return m_Properties.height; }
		ImageFormat GetFormat() override { return m_Properties.format; }
		ImageProperties& GetProperties() override { return m_Properties; }

		void SetData(void* data, uint32_t size) override;
		void* GetData() override { return &m_Buffer; }
		uint32_t GetDataSize() override { return m_Buffer ? m_Buffer->GetSize() : 0; }

		bool operator==(const Image& other) const override { return m_ImageView == ((VulkanImage&)other).GetImageView(); }

		inline VkImageView GetImageView() { return m_ImageView; }
		inline VkImage GetImage() { return m_Image; }

	private:
		ImageProperties m_Properties;
		Shared<VulkanBuffer> m_Buffer;
		VkImageUsageFlags m_UsageFlags;
		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
	};

	namespace VulkanUtils
	{
		VkFormat GetVulkanFormat(ImageFormat format);
		ImageFormat GetImageFormat(VkFormat format);
	}
}