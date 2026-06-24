#include "pxpch.h"
#include "VulkanTexture.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"

#include <stb_image/stb_image.h>

namespace Paradox
{
	VulkanTexture::VulkanTexture(const std::string& debugName, const std::filesystem::path& filePath)
	{
		m_Properties.debugName = debugName;
		Create(filePath);
	}

	VulkanTexture::VulkanTexture(const TextureProperties& props, const std::filesystem::path& filePath)
		: m_Properties(props)
	{
		Create(filePath);
	}

	VulkanTexture::~VulkanTexture()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
		vkDestroySampler(device, m_Sampler, nullptr);
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkDestroyImage(device, m_Image, nullptr);
		vkFreeMemory(device, m_ImageMemory, nullptr);
	}

	//TODO: Should find a better name for this :/
	void VulkanTexture::Create(const std::filesystem::path& filePath)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		PX_CORE_ASSERT(pixels, "Failed to load Texture.");

		VkDeviceSize size = width * height * 4;
		Shared<VulkanBuffer> stagingBuffer = CreateShared<VulkanBuffer>(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer->SetData(pixels, size);
		stbi_image_free(pixels);

		m_Properties.width = width;
		m_Properties.height = height;

		VkDevice device = VulkanDevice::Get().GetDevice();
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Properties.width;
		imageInfo.extent.height = m_Properties.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = GetVulkanFormat(m_Properties.format);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		VK_CHECK_RESULT(vkCreateImage(device, &imageInfo, nullptr, &m_Image));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_IMAGE, m_Image, m_Properties.debugName);

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_Image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &m_ImageMemory));
		VK_CHECK_RESULT(vkBindImageMemory(device, m_Image, m_ImageMemory, 0));

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
		vkCmdCopyBufferToImage(cmdBuffer, stagingBuffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
		VulkanDevice::Get().EndSingleTimeCommands(cmdBuffer);
		VulkanUtils::TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = GetVulkanFormat(m_Properties.format);
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &m_ImageView));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_IMAGE_VIEW, m_ImageView, m_Properties.debugName + " (ImageView)");

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = GetVulkanFilter(m_Properties.magFilter);
		samplerInfo.minFilter = GetVulkanFilter(m_Properties.minFilter);
		samplerInfo.addressModeU = GetVulkanWrap(m_Properties.wrap);
		samplerInfo.addressModeV = GetVulkanWrap(m_Properties.wrap);
		samplerInfo.addressModeW = GetVulkanWrap(m_Properties.wrap);

		samplerInfo.anisotropyEnable = m_Properties.anisotropicFiltering;
		if (samplerInfo.anisotropyEnable)
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(VulkanDevice::Get().GetPhysicalDevice(), &physicalDeviceProperties);
			samplerInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
		}

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = false;
		samplerInfo.compareEnable = false;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.f;
		samplerInfo.minLod = 0.f;
		samplerInfo.maxLod = 0.f;
		VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_SAMPLER, m_Sampler, m_Properties.debugName + " (Sampler)");

		PX_CORE_TRACE("Created Texture: {0} ({1}x{2})", m_Properties.debugName, m_Properties.width, m_Properties.height);
	}

	VkFormat VulkanTexture::GetVulkanFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::SRGBA: return VK_FORMAT_R8G8B8A8_SRGB;
		case TextureFormat::RGBA: return VK_FORMAT_R8G8B8A8_UNORM;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFormat.");
		return VK_FORMAT_UNDEFINED;
	}

	VkFilter VulkanTexture::GetVulkanFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest: return VK_FILTER_NEAREST;
		case TextureFilter::Linear: return VK_FILTER_LINEAR;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFilter.");
		return VK_FILTER_NEAREST;
	}

	VkSamplerAddressMode VulkanTexture::GetVulkanWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case TextureWrap::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case TextureWrap::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case TextureWrap::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		}

		PX_CORE_ASSERT(false, "Invalid TextureWrap.");
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}
