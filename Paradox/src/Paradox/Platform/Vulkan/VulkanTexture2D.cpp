#include "pxpch.h"
#include "VulkanTexture2D.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"

#include <stb_image/stb_image.h>

namespace Paradox
{
	VulkanTexture2D::VulkanTexture2D(const std::string& debugName, const std::filesystem::path& filePath)
	{
		m_Properties.debugName = debugName;
		Create(filePath);
	}

	VulkanTexture2D::VulkanTexture2D(const TextureProperties& props, const std::filesystem::path& filePath)
		: m_Properties(props)
	{
		Create(filePath);
	}

	VulkanTexture2D::VulkanTexture2D(const TextureProperties& props, Shared<Image> image)
		: m_Properties(props), m_Image(image)
	{
		m_Properties.width = image->GetWidth();
		m_Properties.height = image->GetHeight();
		m_Properties.format = image->GetFormat();
		CreateSampler();
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
		vkDestroySampler(device, m_Sampler, nullptr);
	}

	//TODO: Should find a better name for this :/
	void VulkanTexture2D::Create(const std::filesystem::path& filePath)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		PX_CORE_ASSERT(pixels, "Failed to load Texture.");

		VkDeviceSize size = width * height * 4;

		m_Properties.width = width;
		m_Properties.height = height;

		ImageProperties imageProps = {};
		imageProps.width = width;
		imageProps.height = height;
		imageProps.format = m_Properties.format;
		imageProps.usage = ImageUsage::Texture;
		imageProps.debugName = m_Properties.debugName;
		m_Image = Image::Create(imageProps);
		m_Image->SetData(pixels, size);

		stbi_image_free(pixels);

		CreateSampler();

		PX_CORE_TRACE("Created Texture: {0} ({1}x{2})", m_Properties.debugName, m_Properties.width, m_Properties.height);
	}

	void VulkanTexture2D::CreateSampler()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = GetVulkanFilter(m_Properties.magFilter);
		samplerInfo.minFilter = GetVulkanFilter(m_Properties.minFilter);
		VkSamplerAddressMode wrapMode = GetVulkanWrap(m_Properties.wrap);
		samplerInfo.addressModeU = wrapMode;
		samplerInfo.addressModeV = wrapMode;
		samplerInfo.addressModeW = wrapMode;

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
		VK_CHECK_RESULT(vkCreateSampler(VulkanDevice::Get().GetDevice(), &samplerInfo, nullptr, &m_Sampler));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_SAMPLER, m_Sampler, m_Properties.debugName + " (Sampler)");
	}

	VkFilter VulkanTexture2D::GetVulkanFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest: return VK_FILTER_NEAREST;
		case TextureFilter::Linear: return VK_FILTER_LINEAR;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFilter.");
		return VK_FILTER_NEAREST;
	}

	VkSamplerAddressMode VulkanTexture2D::GetVulkanWrap(TextureWrap wrap)
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
