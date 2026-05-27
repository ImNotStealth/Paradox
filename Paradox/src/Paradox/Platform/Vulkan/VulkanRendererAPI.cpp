#include "pxpch.h"
#include "VulkanRendererAPI.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#include "Paradox/Platform/Vulkan/VulkanPipeline.h"
#include "Paradox/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanIndexBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBuffer.h"
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
        Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(swapchain->GetSwapChainRenderPass());

        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkCommandBuffer cmdBuffer = swapchain->GetCommandBuffer(swapchain->GetCurrentFrameIndex());
        VkResult beginCmdBufferResult = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
        PX_CORE_ASSERT(beginCmdBufferResult == VK_SUCCESS, "Failed to begin recording Command Buffer.");

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
        renderPassBeginInfo.framebuffer = swapchain->GetCurrentFramebuffer();
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = swapchain->GetExtent();

        VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipeline());

        VkViewport viewport = {};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = (float)swapchain->GetExtent().width;
        viewport.height = (float)swapchain->GetExtent().height;
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchain->GetExtent();
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		Shared<VulkanShader> shader = std::static_pointer_cast<VulkanShader>(vulkanPipeline->GetProperties().shader);
        if (shader->HasUniforms())
        {
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkWriteDescriptorSet> writes;
            bufferInfos.reserve(shader->GetUniforms().size());
            writes.reserve(shader->GetUniforms().size());

            for (const auto& [binding, uniformEntry] : shader->GetUniforms())
            {
                Shared<VulkanBuffer> ubo = std::static_pointer_cast<VulkanUniformBuffer>(shader->GetUniform(binding)->Get(swapchain->GetCurrentFrameIndex()))->GetBuffer();
                VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
                bufferInfo.buffer = ubo->GetBuffer();
                bufferInfo.range = ubo->GetSize();
                bufferInfo.offset = 0;

                VkWriteDescriptorSet& write = writes.emplace_back();
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = shader->GetDescriptorSets()[swapchain->GetCurrentFrameIndex()];
                write.dstBinding = binding;
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo = &bufferInfo;
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

        VkResult endCmdBufferResult = vkEndCommandBuffer(cmdBuffer);
        PX_CORE_ASSERT(endCmdBufferResult == VK_SUCCESS, "Failed to record Command Buffer.");
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