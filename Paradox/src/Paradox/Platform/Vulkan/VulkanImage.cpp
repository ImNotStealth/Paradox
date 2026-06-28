#include "pxpch.h"
#include "VulkanImage.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	VulkanImage::VulkanImage(const ImageProperties& props)
		: m_Properties(props)
	{
		bool hasDepth = props.format == ImageFormat::Depth32F;

		VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
		if (props.usage == ImageUsage::Attachment)
			usageFlags |= hasDepth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		else if (props.usage == ImageUsage::Texture)
			usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VkFormat format = VulkanUtils::GetVulkanFormat(props.format);
		VulkanUtils::CreateImage(m_Image, props.width, props.height, format, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ImageMemory, props.debugName);

		VkImageAspectFlagBits aspectFlags = hasDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		VulkanUtils::CreateImageView(m_Image, m_ImageView, VK_IMAGE_VIEW_TYPE_2D, format, aspectFlags, props.debugName + " (ImageView)");
	}

	VulkanImage::~VulkanImage()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkDestroyImage(device, m_Image, nullptr);
		vkFreeMemory(device, m_ImageMemory, nullptr);
	}

	void VulkanImage::SetData(void* data, uint32_t size)
	{
		if (!m_Buffer || m_Buffer->GetSize() < size)
			m_Buffer = CreateShared<VulkanBuffer>(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		m_Buffer->SetData(data, size);

		VulkanUtils::TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VkCommandBuffer cmdBuffer = VulkanDevice::Get().BeginSingleTimeCommands();
		VkBufferImageCopy imageCopyRegion = {};
		imageCopyRegion.bufferOffset = 0;
		imageCopyRegion.bufferRowLength = 0;
		imageCopyRegion.bufferImageHeight = 0;
		imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.imageSubresource.mipLevel = 0;
		imageCopyRegion.imageSubresource.baseArrayLayer = 0;
		imageCopyRegion.imageSubresource.layerCount = 1;
		imageCopyRegion.imageOffset = { 0, 0, 0 };
		imageCopyRegion.imageExtent = { m_Properties.width, m_Properties.height, 1 };
		vkCmdCopyBufferToImage(cmdBuffer, m_Buffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
		VulkanDevice::Get().EndSingleTimeCommands(cmdBuffer);
	}

	namespace VulkanUtils
	{
		VkFormat GetVulkanFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::SRGBA: return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::RGBA: return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::BGRA: return VK_FORMAT_B8G8R8A8_UNORM;
			case ImageFormat::Depth32F: return VK_FORMAT_D32_SFLOAT;
			}

			PX_CORE_ASSERT(false, "Invalid ImageFormat.");
			return VK_FORMAT_UNDEFINED;
		}

		ImageFormat GetImageFormat(VkFormat format)
		{
			switch (format)
			{
			case VK_FORMAT_R8G8B8A8_SRGB: return ImageFormat::SRGBA;
			case VK_FORMAT_R8G8B8A8_UNORM: return ImageFormat::RGBA;
			case VK_FORMAT_B8G8R8A8_UNORM: return ImageFormat::BGRA;
			case VK_FORMAT_D32_SFLOAT: return ImageFormat::Depth32F;
			}

			PX_CORE_ASSERT(false, "Invalid VkFormat.");
			return ImageFormat::RGBA;
		}
	}
}
