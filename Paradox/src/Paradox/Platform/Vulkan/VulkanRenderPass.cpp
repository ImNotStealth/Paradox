#include "pxpch.h"
#include "VulkanRenderPass.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"

namespace Paradox
{
	VulkanRenderPass::VulkanRenderPass(const RenderPassProperties& props)
		: m_Properties(props)
	{
        PX_PROFILE_FUNCTION();

        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());

		std::vector<VkAttachmentDescription> attachmentDescs;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		std::vector<VkSubpassDependency> dependencies;

		VkAttachmentReference depthAttachmentRef = {};
        bool hasDepth = false;

        uint32_t attachmentIndex = 0;
        for (ImageFormat format : props.attachments)
        {
            if (ImageUtils::IsDepthFormat(format))
            {
                VkAttachmentDescription& depthAttachment = attachmentDescs.emplace_back();
                depthAttachment.format = VulkanUtils::GetVulkanFormat(format);
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = props.clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = props.clearColor ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                depthAttachmentRef.attachment = attachmentIndex;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDependency& subpassDependency = dependencies.emplace_back();
                subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                subpassDependency.dstSubpass = 0;
                subpassDependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                subpassDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                subpassDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                subpassDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                hasDepth = true;
            }
            else
            {
                VkAttachmentDescription& colorAttachment = attachmentDescs.emplace_back();
                colorAttachment.format = VulkanUtils::GetVulkanFormat(format);
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = props.clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = props.clearColor ? VK_IMAGE_LAYOUT_UNDEFINED : (props.swapchainTarget ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                colorAttachment.finalLayout = props.swapchainTarget ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VkAttachmentReference& colorAttachmentRef = colorAttachmentRefs.emplace_back();
                colorAttachmentRef.attachment = attachmentIndex;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkSubpassDependency& subpassDependency = dependencies.emplace_back();
                subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                subpassDependency.dstSubpass = 0;
                subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpassDependency.srcAccessMask = 0;
                subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
            attachmentIndex++;
		}

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
        subpassDescription.pColorAttachments = colorAttachmentRefs.data();
        if (hasDepth)
            subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = attachmentDescs.size();
        renderPassCreateInfo.pAttachments = attachmentDescs.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        
        renderPassCreateInfo.dependencyCount = (uint32_t)dependencies.size();
        renderPassCreateInfo.pDependencies = dependencies.data();

        VK_CHECK_RESULT(vkCreateRenderPass(VulkanDevice::Get().GetDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass));
        VulkanUtils::SetDebugName(VK_OBJECT_TYPE_RENDER_PASS, m_RenderPass, m_Properties.debugName);

        PX_CORE_TRACE("Created RenderPass: {0}", m_Properties.debugName);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
        PX_PROFILE_FUNCTION();

        VkDevice device = VulkanDevice::Get().GetDevice();
        vkDeviceWaitIdle(device);
        vkDestroyRenderPass(device, m_RenderPass, nullptr);
        PX_CORE_TRACE("Destroyed RenderPass: {0}", m_Properties.debugName);
	}
}
