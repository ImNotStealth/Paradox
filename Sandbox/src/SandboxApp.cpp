#include <Paradox.h>

#include <Paradox/Events/ApplicationEvents.h>
#include <Paradox/Renderer/Shader.h>
#include <Paradox/Platform/Vulkan/VulkanDevice.h>
#include <Paradox/Platform/Vulkan/VulkanSwapChain.h>
#include <Paradox/Platform/Vulkan/VulkanRenderPass.h>
#include <Paradox/Platform/Vulkan/VulkanPipeline.h>
#include <Paradox/Platform/Vulkan/VulkanVertexBuffer.h>
#include <Paradox/Platform/Vulkan/VulkanIndexBuffer.h>
#include <vulkan/vulkan.h>
#include <cstdint>
#include <limits>
#include <fstream>
#include <glm/glm.hpp>

const int MAX_FRAMES_IN_FLIGHT = 2;

using namespace Paradox;

class SandboxApp : public Application
{
public:
    SandboxApp(const WindowCreateProperties& windowProps)
        : Application(windowProps) {}

    void Run() override
    {
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> m_Vertices = {
        {{-0.5f, -0.5f}, {1.f, 0.f, 0.f}},
        {{ 0.5f, -0.5f}, {0.f, 1.f, 0.f}},
        {{ 0.5f,  0.5f}, {0.f, 0.f, 1.f}},
        {{-0.5f,  0.5f}, {1.f, 1.f, 1.f}}
    };

    std::vector<uint16_t> m_Indices = { 0, 1, 2, 2, 3, 0 };

    Shared<Pipeline> m_Pipeline = nullptr;
    Shared<VertexBuffer> m_VertexBuffer = nullptr;
    Shared<IndexBuffer> m_IndexBuffer = nullptr;
    VkCommandPool m_CommandPool = {};
    std::vector<VkCommandBuffer> m_CommandBuffers;
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
    uint32_t m_CurrentFrame = 0;
    bool m_FramebufferResized = false;

private:
    void InitVulkan()
    {
        Shared<Shader> shader = CreateShared<Shader>("Default Shader", "shaders/compiled/shader.vert.spv", "shaders/compiled/shader.frag.spv");
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());

        PipelineProperties pipelineProps = {};
        pipelineProps.shader = shader;
        pipelineProps.renderPass = swapchain->GetSwapChainRenderPass();
        pipelineProps.debugName = "Default Pipeline";
        m_Pipeline = Pipeline::Create(pipelineProps);

        CreateCommandPool();

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Dynamic);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Dynamic);

        CreateCommandBuffers();
        CreateSyncObjects();
    }

    bool OnResize(WindowResizeEvent& e) override
    {
        m_FramebufferResized = true;
        return false;
    }

    void CreateCommandPool()
    {
        VulkanDevice::QueueFamilyIndices familyIndices = VulkanDevice::Get().GetQueueFamilyIndices();
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = familyIndices.graphicsFamily;

        VkResult result = vkCreateCommandPool(VulkanDevice::Get().GetDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool);
        PX_ASSERT(result == VK_SUCCESS, "Failed to create Command Pool.");
    }

    void CreateCommandBuffers()
    {
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

        VkResult result = vkAllocateCommandBuffers(VulkanDevice::Get().GetDevice(), &allocInfo, m_CommandBuffers.data());
        PX_ASSERT(result == VK_SUCCESS, "Failed to allocate Command Buffers.");
    }
    
    void CreateSyncObjects()
    {
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());

        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(swapchain->GetImageCount());
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            bool createSuccess = vkCreateSemaphore(VulkanDevice::Get().GetDevice(), &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) == VK_SUCCESS &&
                vkCreateFence(VulkanDevice::Get().GetDevice(), &fenceCreateInfo, nullptr, &m_InFlightFences[i]) == VK_SUCCESS;
            PX_ASSERT(createSuccess, "Failed to create sync objects for a frame.");
        }

        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
        {
            VkResult createResult = vkCreateSemaphore(VulkanDevice::Get().GetDevice(), &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
            PX_ASSERT(createResult == VK_SUCCESS, "Failed to create RenderFinished semaphore.");
        }
    }

    void RecordCommandBuffer(const VkCommandBuffer& cmdBuffer, uint32_t imageIndex)
    {
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());
        Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(swapchain->GetSwapChainRenderPass());

        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkResult beginCmdBufferResult = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
        PX_ASSERT(beginCmdBufferResult == VK_SUCCESS, "Failed to begin recording Command Buffer.");

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
        renderPassBeginInfo.framebuffer = swapchain->GetFramebuffer(imageIndex);
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = swapchain->GetExtent();

        VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        Shared<VulkanPipeline> pipeline = std::static_pointer_cast<VulkanPipeline>(m_Pipeline);

        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

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

        Shared<VulkanVertexBuffer> vertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(m_VertexBuffer);
        Shared<VulkanIndexBuffer> indexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(m_IndexBuffer);

        VkBuffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(cmdBuffer, m_IndexBuffer->GetCount(), 1, 0, 0, 0);
        vkCmdEndRenderPass(cmdBuffer);

        VkResult endCmdBufferResult = vkEndCommandBuffer(cmdBuffer);
        PX_ASSERT(endCmdBufferResult == VK_SUCCESS, "Failed to record Command Buffer.");
    }

    void RecreateSwapchain()
    {
        while (GetWindow().GetWidth() == 0 || GetWindow().GetHeight() == 0)
        {
            GetWindow().WaitEvents();
        }

        GetWindow().GetGraphicsContext()->WaitIdle();
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());
        swapchain->OnResize(GetWindow().GetWidth(), GetWindow().GetHeight());
    }

    void MainLoop()
    {
        while (IsRunning())
        {
            GetWindow().PollEvents();
            DrawFrame();
		}

        vkDeviceWaitIdle(VulkanDevice::Get().GetDevice());
    }

    void DrawFrame()
    {
		m_Vertices[0].pos.x += 0.00005f;
		m_VertexBuffer->SetData(m_Vertices.data(), (uint32_t)(sizeof(Vertex) * m_Vertices.size()));

        if (m_Vertices[0].pos.x > -0.4f)
        {
            m_Indices = { 0, 1, 2, 0, 1, 3 };
            m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size());
        }

        vkWaitForFences(VulkanDevice::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(VulkanDevice::Get().GetDevice(), swapchain->GetSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return;
        }
        else
            PX_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image.");

        vkResetFences(VulkanDevice::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult queueResult = vkQueueSubmit(VulkanDevice::Get().GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
        PX_ASSERT(queueResult == VK_SUCCESS, "Failed to submit to draw Command Buffer.");
    
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = { swapchain->GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(VulkanDevice::Get().GetPresentQueue(), &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
        {
            m_FramebufferResized = false;
            RecreateSwapchain();
        }
        else
            PX_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image.");

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Cleanup()
    {
        PX_INFO("Cleanup");

        VkDevice device = VulkanDevice::Get().GetDevice();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, m_InFlightFences[i], nullptr);
        }

        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());
        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
            vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);

        vkDestroyCommandPool(device, m_CommandPool, nullptr);
    }
};

Application* Paradox::CreateApplication()
{
    WindowCreateProperties createProps;
    createProps.title = "Sandbox";
    createProps.width = 800;
    createProps.height = 600;
    return new SandboxApp(createProps);
}