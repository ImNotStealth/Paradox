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

		VulkanUtils::CreateImage(m_Image, width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ImageMemory, m_Props.debugName);
		VulkanUtils::CreateImageView(m_Image, m_ImageView, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_Props.debugName + " (ImageView)");

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