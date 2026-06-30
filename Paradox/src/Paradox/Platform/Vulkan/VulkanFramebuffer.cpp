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
		Resize(m_Props.width, m_Props.height);
		PX_CORE_TRACE("Created Framebuffer: {0} ({1}x{2})", m_Props.debugName, m_Props.width, m_Props.height);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
		PX_CORE_TRACE("Destroyed Framebuffer: {0}", m_Props.debugName);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		VkDevice device = VulkanDevice::Get().GetDevice();

		if (m_Framebuffer != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(device);
			vkDestroyFramebuffer(device, m_Framebuffer, nullptr);

#ifdef PX_INCLUDE_IMGUI
			if (!m_Props.swapchainTarget)
				ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);	
#endif

			m_Attachments.clear();
		}

		if (m_Props.swapchainTarget)
		{
			Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
			m_RenderPass = swapchain->GetSwapChainRenderPass();
		}
		else
		{
			std::vector<VkImageView> imageViews;
			imageViews.reserve(m_Props.attachments.size());
			
			if (m_Props.attachmentImages.empty())
			{
				for (ImageFormat format : m_Props.attachments)
				{
					ImageProperties imageProps = {};
					imageProps.width = width;
					imageProps.height = height;
					imageProps.usage = ImageUsage::Attachment;
					imageProps.format = format;
					imageProps.debugName = m_Props.debugName;
					Shared<Image> attachment = Image::Create(imageProps);
					imageViews.push_back(std::static_pointer_cast<VulkanImage>(attachment)->GetImageView());
					m_Attachments.emplace_back(attachment);
				}
			}
			else
			{
				for (Shared<Image> attachment : m_Props.attachmentImages)
				{
					imageViews.push_back(std::static_pointer_cast<VulkanImage>(attachment)->GetImageView());
					m_Attachments.emplace_back(attachment);
				}
			}

			RenderPassProperties renderPassProps = {};
			renderPassProps.clearColor = m_Props.clear;
			renderPassProps.attachments = m_Props.attachments;
			renderPassProps.swapchainTarget = m_Props.swapchainTarget;
			renderPassProps.debugName = m_Props.debugName + " (RenderPass)";
			m_RenderPass = RenderPass::Create(renderPassProps);

			Shared<VulkanRenderPass> vulkanRenderPass = std::static_pointer_cast<VulkanRenderPass>(m_RenderPass);
			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = vulkanRenderPass->GetRenderPass();
			framebufferCreateInfo.attachmentCount = imageViews.size();
			framebufferCreateInfo.pAttachments = imageViews.data();
			framebufferCreateInfo.width = width;
			framebufferCreateInfo.height = height;
			framebufferCreateInfo.layers = 1;
			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_Framebuffer));
			VulkanUtils::SetDebugName(VK_OBJECT_TYPE_FRAMEBUFFER, m_Framebuffer, m_Props.debugName + " (Framebuffer)");

			for (const Shared<Image>& attachment : m_Attachments)
			{
				if (attachment->GetFormat() == ImageFormat::Depth32F)
					continue;

				Shared<VulkanImage> vulkanImage = std::static_pointer_cast<VulkanImage>(attachment);
				VulkanUtils::TransitionImageLayout(vulkanImage->GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}

#ifdef PX_INCLUDE_IMGUI
			m_DescriptorSet = ImGui_ImplVulkan_AddTexture(std::static_pointer_cast<VulkanImage>(m_Attachments[0])->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
#endif
		}

		m_Props.width = width;
		m_Props.height = height;
	}
}