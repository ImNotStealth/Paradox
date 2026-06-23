#include "pxpch.h"
#include "VulkanFramebuffer.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"

#ifdef PX_INCLUDE_IMGUI
#include <backends/imgui_impl_vulkan.h>
#endif

namespace Paradox
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& props)
		: m_Props(props)
	{
		OnResize(m_Props.width, m_Props.height);
		PX_CORE_TRACE("Created Framebuffer: {0} ({1}x{2})", m_Props.debugName, m_Props.width, m_Props.height);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
		vkDestroyImage(device, m_Image, nullptr);
		vkDestroyImageView(device, m_ImageView, nullptr);
		vkFreeMemory(device, m_ImageMemory, nullptr);
		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
		PX_CORE_TRACE("Destroyed Framebuffer: {0}", m_Props.debugName);
	}

	void VulkanFramebuffer::OnResize(uint32_t width, uint32_t height)
	{
		VkDevice device = VulkanDevice::Get().GetDevice();

		if (m_Image != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(device);
			vkDestroyImage(device, m_Image, nullptr);
			vkDestroyImageView(device, m_ImageView, nullptr);
			vkFreeMemory(device, m_ImageMemory, nullptr);
			vkDestroyFramebuffer(device, m_Framebuffer, nullptr);

#ifdef PX_INCLUDE_IMGUI
			ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
#endif
		}

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateImage(device, &imageInfo, nullptr, &m_Image));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_IMAGE, m_Image, m_Props.debugName);

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_Image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &m_ImageMemory));
		VK_CHECK_RESULT(vkBindImageMemory(device, m_Image, m_ImageMemory, 0));

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &m_ImageView));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_IMAGE_VIEW, m_ImageView, m_Props.debugName + " (ImageView)");

		RenderPassProperties renderPassProps = {};
		renderPassProps.clearColor = m_Props.clear;
		renderPassProps.swapchainTarget = m_Props.swapchainTarget;
		renderPassProps.debugName = m_Props.debugName + " (RenderPass)";
		m_RenderPass = RenderPass::Create(renderPassProps);

		Shared<VulkanRenderPass> vulkanRenderPass = std::static_pointer_cast<VulkanRenderPass>(m_RenderPass);
		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = vulkanRenderPass->GetRenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &m_ImageView;
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;
		VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_Framebuffer));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_FRAMEBUFFER, m_Framebuffer, m_Props.debugName + " (Framebuffer)");

		VulkanUtils::TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

#ifdef PX_INCLUDE_IMGUI
		m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
#endif

		m_Props.width = width;
		m_Props.height = height;
	}
}