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
		for (const FramebufferAttachment& attachment : m_Props.attachments)
		{
			if (attachment.existingImage)
			{
				m_Attachments.emplace_back(attachment.existingImage);
				continue;
			}

			ImageProperties imageProps = {};
			imageProps.width = props.width;
			imageProps.height = props.height;
			imageProps.usage = ImageUsage::Attachment;
			imageProps.format = attachment.format;
			imageProps.debugName = m_Props.debugName;
			m_Attachments.emplace_back(Image::Create(imageProps));
		}

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
		}

		if (m_Props.swapchainTarget)
		{
			Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
			m_RenderPass = swapchain->GetSwapChainRenderPass();
		}
		else
		{
			std::vector<VkImageView> imageViews;
			std::vector<ImageFormat> renderPassAttachments;
			imageViews.reserve(m_Props.attachments.size());
			renderPassAttachments.reserve(m_Props.attachments.size());
			
			for (Shared<Image>& image : m_Attachments)
			{
				bool isExistingImage = false;
				for (const FramebufferAttachment& attachment : m_Props.attachments)
				{
					if (attachment.existingImage == image)
					{
						isExistingImage = true;
						break;
					}
				}

				if (!isExistingImage)
					image->Resize(width, height);

				renderPassAttachments.push_back(image->GetFormat());
				imageViews.push_back(std::static_pointer_cast<VulkanImage>(image)->GetImageView());
			}

			RenderPassProperties renderPassProps = {};
			renderPassProps.clearColor = m_Props.clear;
			renderPassProps.attachments = renderPassAttachments;
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
				if (VulkanUtils::IsDepthFormat(attachment->GetFormat()))
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