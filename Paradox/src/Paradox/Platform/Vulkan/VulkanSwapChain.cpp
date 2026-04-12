#include "pxpch.h"
#include "VulkanSwapChain.h"

#include "Paradox/Core/Window.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Paradox
{
	VulkanSwapChain::~VulkanSwapChain()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();

		for (VulkanImage& img : m_Images)
			vkDestroyImageView(device, img.ImageView, nullptr);
		m_Images.clear();

		vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
		vkDestroySurfaceKHR(VulkanContext::GetVkInstance(), m_Surface, nullptr);
	}

	void VulkanSwapChain::Init(Window* window)
	{
		VkResult result = glfwCreateWindowSurface(VulkanContext::GetVkInstance(), (GLFWwindow*)window->GetHandle(), nullptr, &m_Surface);
		PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Surface");
	}

	void VulkanSwapChain::Create(uint32_t width, uint32_t height, bool vsync)
	{
		m_Width = width;
		m_Height = height;
		m_VSync = vsync;

		VkPhysicalDevice physicalDevice = VulkanDevice::Get().GetPhysicalDevice();
		FindFormatAndColorSpace();

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, VulkanDevice::Get().GetQueueFamilyIndices().GraphicsFamily, m_Surface, &presentSupport);
		if (presentSupport == VK_FALSE)
			PX_CORE_ERROR("Vulkan Present Queue not supported.");

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &capabilities);

		VkExtent2D extent;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			extent = capabilities.currentExtent;
		else
		{
			extent = { width, height };
			extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		}
		m_Extent = extent;

		VkPresentModeKHR presentMode = GetPresentMode();

		m_ImageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && m_ImageCount > capabilities.maxImageCount)
			m_ImageCount = capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = m_ImageCount;
		createInfo.imageFormat = m_ColorFormat;
		createInfo.imageColorSpace = m_ColorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = m_OldSwapChain;

		VkDevice device = VulkanDevice::Get().GetDevice();
		VkResult swapChainResult = vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain);
		PX_ASSERT(swapChainResult == VK_SUCCESS, "Failed to create swapchain.");

		if (m_OldSwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, m_OldSwapChain, nullptr);
			m_OldSwapChain = VK_NULL_HANDLE;

			for (size_t i = 0; i < m_Images.size(); i++)
				vkDestroyImageView(Paradox::VulkanDevice::Get().GetDevice(), m_Images[i].ImageView, nullptr);
			
			m_Images.clear();
		}

		// Images
		vkGetSwapchainImagesKHR(Paradox::VulkanDevice::Get().GetDevice(), m_SwapChain, &m_ImageCount, nullptr);
		m_Images.resize(m_ImageCount);
		std::vector<VkImage> vkImages(m_ImageCount);
		vkGetSwapchainImagesKHR(Paradox::VulkanDevice::Get().GetDevice(), m_SwapChain, &m_ImageCount, vkImages.data());

		for (size_t i = 0; i < m_ImageCount; i++)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = vkImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_ColorFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(Paradox::VulkanDevice::Get().GetDevice(), &createInfo, nullptr, &m_Images[i].ImageView);
			PX_ASSERT(result == VK_SUCCESS, "Failed to create image view.");
		
			m_Images[i].Image = vkImages[i];
		}

		PX_CORE_TRACE("Created Vulkan SwapChain");
	}

	void VulkanSwapChain::OnResize(uint32_t width, uint32_t height)
	{
		vkDeviceWaitIdle(VulkanDevice::Get().GetDevice());
		m_OldSwapChain = m_SwapChain;
		m_SwapChain = VK_NULL_HANDLE;
		Create(width, height, m_VSync);
	}

	void VulkanSwapChain::FindFormatAndColorSpace()
	{
		VkPhysicalDevice physicalDevice = VulkanDevice::Get().GetPhysicalDevice();

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);

		PX_CORE_ASSERT(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> formats(formatCount);
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, formats.data());

		// If only 1 format exists and it's undefined, there's no prefered format so we default to B8G8R8A8_SRGB
		if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			m_ColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
			m_ColorSpace = formats[0].colorSpace;
			return;
		}

		// Select format and color space that match VK_FORMAT_B8G8R8A8_SRGB
		for (VkSurfaceFormatKHR& surfaceFormat : formats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
			{
				m_ColorFormat = surfaceFormat.format;
				m_ColorSpace = surfaceFormat.colorSpace;
				return;
			}
		}

		// If none found, default to first color format and color space
		m_ColorFormat = formats[0].format;
		m_ColorSpace = formats[0].colorSpace;
	}

	VkPresentModeKHR VulkanSwapChain::GetPresentMode()
	{
		if (!m_VSync)
		{
			VkPhysicalDevice device = VulkanDevice::Get().GetPhysicalDevice();
			uint32_t presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

			std::vector<VkPresentModeKHR> modes(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, modes.data());

			if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end())
				return VK_PRESENT_MODE_MAILBOX_KHR;

			if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != modes.end())
				return VK_PRESENT_MODE_IMMEDIATE_KHR;
		}

		// VSync Mode
		return VK_PRESENT_MODE_FIFO_KHR;
	}
}