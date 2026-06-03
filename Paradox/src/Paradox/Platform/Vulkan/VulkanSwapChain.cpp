#include "pxpch.h"
#include "VulkanSwapChain.h"

#include "Paradox/Core/Window.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanContext.h"
#include "Paradox/Renderer/Renderer.h"
#include "Paradox/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Paradox
{
	VulkanSwapChain::~VulkanSwapChain()
	{
		PX_CORE_TRACE("Destroy Vulkan SwapChain");

		VkDevice device = VulkanDevice::Get().GetDevice();

		for (VulkanImage& img : m_Images)
			vkDestroyImageView(device, img.imageView, nullptr);

		for (VkFramebuffer& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(VulkanDevice::Get().GetDevice(), framebuffer, nullptr);

		for (size_t i = 0; i < Renderer::GetMaxFramesInFlight(); i++)
		{
			vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, m_InFlightFences[i], nullptr);
		}

		for (VkSemaphore& semaphore : m_RenderFinishedSemaphores)
			vkDestroySemaphore(device, semaphore, nullptr);

		m_Images.clear();
		m_Framebuffers.clear();
		m_CommandBuffers.clear();
		m_ImageAvailableSemaphores.clear();
		m_InFlightFences.clear();
		m_RenderFinishedSemaphores.clear();

		if (m_SwapChain != VK_NULL_HANDLE)
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
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, VulkanDevice::Get().GetQueueFamilyIndices().graphicsFamily, m_Surface, &presentSupport);
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
		PX_CORE_ASSERT(swapChainResult == VK_SUCCESS, "Failed to create swapchain.");

		RenderPassProperties renderPassProps = { "SwapChain RenderPass" };
		m_RenderPass = RenderPass::Create(renderPassProps);

		if (m_OldSwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, m_OldSwapChain, nullptr);
			m_OldSwapChain = VK_NULL_HANDLE;

			for (size_t i = 0; i < m_Images.size(); i++)
				vkDestroyImageView(VulkanDevice::Get().GetDevice(), m_Images[i].imageView, nullptr);

			for (VkFramebuffer& framebuffer : m_Framebuffers)
				vkDestroyFramebuffer(VulkanDevice::Get().GetDevice(), framebuffer, nullptr);

			for (size_t i = 0; i < Renderer::GetMaxFramesInFlight(); i++)
			{
				vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
				vkDestroyFence(device, m_InFlightFences[i], nullptr);
			}

			for (VkSemaphore& semaphore : m_RenderFinishedSemaphores)
				vkDestroySemaphore(device, semaphore, nullptr);
			
			m_Images.clear();
			m_Framebuffers.clear();
			m_CommandBuffers.clear();
			m_ImageAvailableSemaphores.clear();
			m_InFlightFences.clear();
			m_RenderFinishedSemaphores.clear();
		}

		// Images
		vkGetSwapchainImagesKHR(VulkanDevice::Get().GetDevice(), m_SwapChain, &m_ImageCount, nullptr);
		m_Images.resize(m_ImageCount);
		std::vector<VkImage> vkImages(m_ImageCount);
		vkGetSwapchainImagesKHR(VulkanDevice::Get().GetDevice(), m_SwapChain, &m_ImageCount, vkImages.data());

		Shared<VulkanRenderPass> vulkanRenderPass = std::static_pointer_cast<VulkanRenderPass>(m_RenderPass);
		m_Framebuffers.resize(m_ImageCount);

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

			VkResult result = vkCreateImageView(VulkanDevice::Get().GetDevice(), &createInfo, nullptr, &m_Images[i].imageView);
			PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image view.");
		
			m_Images[i].image = vkImages[i];

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = vulkanRenderPass->GetRenderPass();
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = &m_Images[i].imageView;
			framebufferCreateInfo.width = m_Extent.width;
			framebufferCreateInfo.height = m_Extent.height;
			framebufferCreateInfo.layers = 1;

			result = vkCreateFramebuffer(VulkanDevice::Get().GetDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffers[i]);
			PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Framebuffer.");
		}

		// Command Buffers
		m_CommandBuffers.resize(m_ImageCount);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = VulkanDevice::Get().GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		VkResult result = vkAllocateCommandBuffers(VulkanDevice::Get().GetDevice(), &allocInfo, m_CommandBuffers.data());
		PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate Command Buffers.");

		// Sync Objects
		m_ImageAvailableSemaphores.resize(Renderer::GetMaxFramesInFlight());
		m_InFlightFences.resize(Renderer::GetMaxFramesInFlight());
		m_RenderFinishedSemaphores.resize(m_ImageCount);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < Renderer::GetMaxFramesInFlight(); i++)
		{
			bool createSuccess = vkCreateSemaphore(VulkanDevice::Get().GetDevice(), &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) == VK_SUCCESS &&
				vkCreateFence(VulkanDevice::Get().GetDevice(), &fenceCreateInfo, nullptr, &m_InFlightFences[i]) == VK_SUCCESS;
			PX_CORE_ASSERT(createSuccess, "Failed to create sync objects for a frame.");
		}

		for (size_t i = 0; i < m_ImageCount; i++)
		{
			VkResult createResult = vkCreateSemaphore(VulkanDevice::Get().GetDevice(), &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
			PX_CORE_ASSERT(createResult == VK_SUCCESS, "Failed to create RenderFinished semaphore.");
		}

		PX_CORE_TRACE("Created Vulkan SwapChain");
	}

	void VulkanSwapChain::OnResize(uint32_t width, uint32_t height)
	{
		PX_CORE_INFO("SwapChain resized: {0}x{1}", width, height);
		vkDeviceWaitIdle(VulkanDevice::Get().GetDevice());
		m_OldSwapChain = m_SwapChain;
		m_SwapChain = VK_NULL_HANDLE;
		Create(width, height, m_VSync);
	}

	void VulkanSwapChain::RequestResize()
	{
		m_ResizeRequested = true;
	}

	void VulkanSwapChain::Begin()
	{
		vkWaitForFences(VulkanDevice::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(VulkanDevice::Get().GetDevice(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImage);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			const Window& window = Application::Get().GetWindow();
			OnResize(window.GetWidth(), window.GetHeight());
			return;
		}
		
		PX_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image.");

		vkResetFences(VulkanDevice::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);
		vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
	}

	void VulkanSwapChain::End()
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentImage] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkResult queueResult = vkQueueSubmit(VulkanDevice::Get().GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
		PX_CORE_ASSERT(queueResult == VK_SUCCESS, "Failed to submit to draw Command Buffer.");

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { m_SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &m_CurrentImage;

		VkResult result = vkQueuePresentKHR(VulkanDevice::Get().GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_ResizeRequested)
		{
			m_ResizeRequested = false;
			const Window& window = Application::Get().GetWindow();
			OnResize(window.GetWidth(), window.GetHeight());
		}
		else
			PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image.");
		m_CurrentFrame = (m_CurrentFrame + 1) % Renderer::GetMaxFramesInFlight();
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