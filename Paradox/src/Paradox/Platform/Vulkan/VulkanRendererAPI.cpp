#include "pxpch.h"
#include "VulkanRendererAPI.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#include "Paradox/Platform/Vulkan/VulkanPipeline.h"
#include "Paradox/Platform/Vulkan/VulkanFramebuffer.h"
#include "Paradox/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanIndexBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBufferSet.h"
#include "Paradox/Platform/Vulkan/VulkanTexture2D.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"
#include "Paradox/Platform/Vulkan/VulkanShader.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	void VulkanRendererAPI::Init()
	{
		PX_CORE_INFO("Init VulkanRendererAPI.");
	}

	void VulkanRendererAPI::BeginRenderPass(const Shared<Pipeline>& pipeline)
	{
		Shared<VulkanPipeline> vulkanPipeline = std::static_pointer_cast<VulkanPipeline>(pipeline);
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
		Shared<VulkanFramebuffer> framebuffer = std::static_pointer_cast<VulkanFramebuffer>(pipeline->GetProperties().framebuffer);

        bool swapchainTarget = framebuffer->GetProperties().swapchainTarget;
        Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(swapchainTarget ? swapchain->GetSwapChainRenderPass() : framebuffer->GetRenderPass());

        VkExtent2D extent = {};
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass->GetRenderPass();

        if (swapchainTarget)
        {
            renderPassBeginInfo.framebuffer = swapchain->GetCurrentFramebuffer();
			extent = swapchain->GetExtent();
        }
        else
        {
            renderPassBeginInfo.framebuffer = framebuffer->GetFramebuffer();
            extent = { framebuffer->GetProperties().width, framebuffer->GetProperties().height };
        }

        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = extent;

		glm::vec4 clearColor = framebuffer->GetProperties().clearColor;

        std::vector<VkClearValue> clearValues;
        VkClearValue clearValue = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
        clearValues.push_back(clearValue);

        for (const FramebufferAttachment& attachment : framebuffer->GetProperties().attachments)
        {
            if (ImageUtils::IsDepthFormat(attachment.format))
            {
                VkClearValue depthClearValue = {};
                depthClearValue.depthStencil = { 1.0f, 0 };
                clearValues.push_back(depthClearValue);
                break;
            }
        }

        renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();

        VkCommandBuffer cmdBuffer = swapchain->GetCommandBuffer(swapchain->GetCurrentFrameIndex());
        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());

        VkViewport viewport = {};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float)extent.width;
        viewport.height = (float)extent.height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		Shared<VulkanShader> shader = std::static_pointer_cast<VulkanShader>(vulkanPipeline->GetProperties().shader);
        if (shader->HasInputs())
        {
			PX_CORE_ASSERT(shader->IsBaked(), "Shader Inputs must be baked before rendering.");

            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfos;
            std::vector<VkWriteDescriptorSet> writes;
            bufferInfos.reserve(shader->GetInputs().size());
            imageInfos.reserve(shader->GetInputs().size());
            writes.reserve(shader->GetInputs().size());

            for (const auto& [binding, entry] : shader->GetInputs())
            {
                if (entry.type == ShaderInputType::UniformBuffer)
                {
                    Shared<VulkanUniformBufferSet> ubo = std::static_pointer_cast<VulkanUniformBufferSet>(entry.data);
                    Shared<VulkanBuffer> uboBuffer = std::static_pointer_cast<VulkanUniformBuffer>(ubo->Get(swapchain->GetCurrentFrameIndex()))->GetBuffer();
                    VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
                    bufferInfo.buffer = uboBuffer->GetBuffer();
                    bufferInfo.range = uboBuffer->GetSize();
                    bufferInfo.offset = 0;

                    VkWriteDescriptorSet& write = writes.emplace_back();
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = shader->GetDescriptorSets()[swapchain->GetCurrentFrameIndex()];
                    write.dstBinding = binding;
                    write.descriptorCount = 1;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    write.pBufferInfo = &bufferInfo;
                }
                else if (entry.type == ShaderInputType::Texture)
                {
                    Shared<VulkanTexture2D> texture = std::static_pointer_cast<VulkanTexture2D>(entry.data);
                    Shared<VulkanImage> image = std::static_pointer_cast<VulkanImage>(texture->GetImage());
                    VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.sampler = texture->GetSampler();
                    imageInfo.imageView = image->GetImageView();

                    VkWriteDescriptorSet& write = writes.emplace_back();
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = shader->GetDescriptorSets()[swapchain->GetCurrentFrameIndex()];
                    write.dstBinding = binding;
                    write.descriptorCount = 1;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.pImageInfo = &imageInfo;
                }
            }

            vkUpdateDescriptorSets(VulkanDevice::Get().GetDevice(), writes.size(), writes.data(), 0, nullptr);
            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 0, 1, &shader->GetDescriptorSets()[swapchain->GetCurrentFrameIndex()], 0, nullptr);
        }
	}

	void VulkanRendererAPI::EndRenderPass()
	{
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
        VkCommandBuffer cmdBuffer = swapchain->GetCommandBuffer(swapchain->GetCurrentFrameIndex());

        vkCmdEndRenderPass(cmdBuffer);
	}

    void VulkanRendererAPI::DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer)
    {
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
        VkCommandBuffer cmdBuffer = swapchain->GetCommandBuffer(swapchain->GetCurrentFrameIndex());

        Shared<VulkanVertexBuffer> vulkanVertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
        Shared<VulkanIndexBuffer> vulkanIndexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer->GetBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(cmdBuffer, vulkanIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmdBuffer, indexBuffer->GetCount(), 1, 0, 0, 0);
    }
}