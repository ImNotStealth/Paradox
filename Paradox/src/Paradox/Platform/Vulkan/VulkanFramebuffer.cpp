#include "pxpch.h"
#include "VulkanFramebuffer.h"

//#include "Paradox/Core/Application.h"
//#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
//#include "Paradox/Platform/Vulkan/VulkanDevice.h"
//
//namespace Paradox
//{
//	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& props)
//		: m_Props(props)
//	{
//		OnResize(m_Props.width, m_Props.height);
//	}
//
//	VulkanFramebuffer::~VulkanFramebuffer()
//	{
//		vkDestroyFramebuffer(VulkanDevice::Get().GetDevice(), m_Framebuffer, nullptr);
//	}
//
//	void VulkanFramebuffer::OnResize(uint32_t width, uint32_t height)
//	{
//		Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
//		Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(m_Props.renderPass);
//		VkFramebufferCreateInfo framebufferCreateInfo = {};
//		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//		framebufferCreateInfo.renderPass = ;
//		framebufferCreateInfo.attachmentCount = 1;
//		framebufferCreateInfo.pAttachments = &m_Images[i].imageView;
//		framebufferCreateInfo.width = m_Props.width;
//		framebufferCreateInfo.height = m_Props.height;
//		framebufferCreateInfo.layers = 1;
//
//		VkResult result = vkCreateFramebuffer(VulkanDevice::Get().GetDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffer);
//		PX_ASSERT(result == VK_SUCCESS, "Failed to create Framebuffer.");
//	}
//}