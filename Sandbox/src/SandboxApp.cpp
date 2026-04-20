#include <Paradox.h>

#include <Paradox/Events/ApplicationEvents.h>
#include <Paradox/Renderer/Shader.h>
#include <Paradox/Platform/Vulkan/VulkanDevice.h>
#include <Paradox/Platform/Vulkan/VulkanSwapChain.h>
#include <Paradox/Platform/Vulkan/VulkanRenderPass.h>
#include <Paradox/Platform/Vulkan/VulkanPipeline.h>
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
    Shared<RenderPass> m_RenderPass = nullptr;
    Shared<Pipeline> m_Pipeline = nullptr;
    std::vector<VkFramebuffer> m_SwapchainFramebuffers;
    VkCommandPool m_CommandPool = {};
    std::vector<VkCommandBuffer> m_CommandBuffers;
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
    uint32_t m_CurrentFrame = 0;
    bool m_FramebufferResized = false;
    VkBuffer m_VertexBuffer = {};
    VkDeviceMemory m_VertexBufferMemory = {};
    VkBuffer m_IndexBuffer = {};
    VkDeviceMemory m_IndexBufferMemory = {};

private:
    void InitVulkan()
    {
        RenderPassProperties props = { "Default Render Pass" };
        m_RenderPass = RenderPass::Create(props);

        Shared<Shader> shader = CreateShared<Shader>("Default Shader", "shaders/compiled/shader.vert.spv", "shaders/compiled/shader.frag.spv");

        PipelineProperties pipelineProps = {};
        pipelineProps.Shader = shader;
        pipelineProps.RenderPass = m_RenderPass;
        pipelineProps.DebugName = "Default Pipeline";
        m_Pipeline = Pipeline::Create(pipelineProps);

        CreateFramebuffers();
        CreateCommandPool();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    bool OnResize(WindowResizeEvent& e) override
    {
        m_FramebufferResized = true;
        return false;
    }

    struct Vertex
    {
        glm::vec2 m_Pos;
        glm::vec3 m_Color;
    };

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.f, 0.f, 0.f}},
        {{ 0.5f, -0.5f}, {0.f, 1.f, 0.f}},
        {{ 0.5f,  0.5f}, {0.f, 0.f, 1.f}},
        {{-0.5f,  0.5f}, {1.f, 1.f, 1.f}}
    };

    const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };

    void CreateFramebuffers()
    {
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());
        Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(m_RenderPass);

        m_SwapchainFramebuffers.resize(swapchain->GetImageCount());
        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
        {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderPass->GetRenderPass();
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &swapchain->GetImages()[i].ImageView;
            framebufferCreateInfo.width = swapchain->GetExtent().width;
            framebufferCreateInfo.height = swapchain->GetExtent().height;
            framebufferCreateInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(VulkanDevice::Get().GetDevice(), &framebufferCreateInfo, nullptr, &m_SwapchainFramebuffers[i]);
            PX_ASSERT(result == VK_SUCCESS, "Failed to create Framebuffer.");
        }
    }

    void CreateCommandPool()
    {
        VulkanDevice::QueueFamilyIndices familyIndices = VulkanDevice::Get().GetQueueFamilyIndices();
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = familyIndices.GraphicsFamily;

        VkResult result = vkCreateCommandPool(VulkanDevice::Get().GetDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool);
        PX_ASSERT(result == VK_SUCCESS, "Failed to create Command Pool.");
    }

    void CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        
        VkBuffer stagingBuffer = {};
        VkDeviceMemory stagingBufferMemory = {};
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanDevice::Get().GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanDevice::Get().GetDevice(), stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
        CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(VulkanDevice::Get().GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanDevice::Get().GetDevice(), stagingBufferMemory, nullptr);
    }

    void CreateIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer = {};
        VkDeviceMemory stagingBufferMemory = {};
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(VulkanDevice::Get().GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanDevice::Get().GetDevice(), stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
        CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(VulkanDevice::Get().GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanDevice::Get().GetDevice(), stagingBufferMemory, nullptr);
    }

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmdBuffer = {};
        vkAllocateCommandBuffers(VulkanDevice::Get().GetDevice(), &allocInfo, &cmdBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(cmdBuffer, &beginInfo);
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = bufferSize;
        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        vkEndCommandBuffer(cmdBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        vkQueueSubmit(VulkanDevice::Get().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(VulkanDevice::Get().GetGraphicsQueue());
        vkFreeCommandBuffers(VulkanDevice::Get().GetDevice(), m_CommandPool, 1, &cmdBuffer);
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties = {};
        vkGetPhysicalDeviceMemoryProperties(VulkanDevice::Get().GetPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        PX_ASSERT(false, "Failed to find suitable memory type.");
        return 0;
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propFlags, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult bufferResult = vkCreateBuffer(VulkanDevice::Get().GetDevice(), &bufferCreateInfo, nullptr, &buffer);
        PX_ASSERT(bufferResult == VK_SUCCESS, "Failed to create Buffer.");

        VkMemoryRequirements memRequirements = {};
        vkGetBufferMemoryRequirements(VulkanDevice::Get().GetDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, propFlags);

        VkResult allocateResult = vkAllocateMemory(VulkanDevice::Get().GetDevice(), &allocInfo, nullptr, &bufferMemory);
        PX_ASSERT(allocateResult == VK_SUCCESS, "Failed to allocate Buffer memory.");

        vkBindBufferMemory(VulkanDevice::Get().GetDevice(), buffer, bufferMemory, 0);
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
        Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(m_RenderPass);

        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkResult beginCmdBufferResult = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
        PX_ASSERT(beginCmdBufferResult == VK_SUCCESS, "Failed to begin recording Command Buffer.");

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
        renderPassBeginInfo.framebuffer = m_SwapchainFramebuffers[imageIndex];
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

        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(cmdBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(cmdBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
        vkCmdEndRenderPass(cmdBuffer);

        VkResult endCmdBufferResult = vkEndCommandBuffer(cmdBuffer);
        PX_ASSERT(endCmdBufferResult == VK_SUCCESS, "Failed to record Command Buffer.");
    }

    void CleanupSwapchain()
    {
        for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++)
            vkDestroyFramebuffer(VulkanDevice::Get().GetDevice(), m_SwapchainFramebuffers[i], nullptr);
    }

    void RecreateSwapchain()
    {
        while (GetWindow().GetWidth() == 0 || GetWindow().GetHeight() == 0)
        {
            GetWindow().WaitEvents();
        }

        GetWindow().GetGraphicsContext()->WaitIdle();
        CleanupSwapchain();
        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());
        swapchain->OnResize(GetWindow().GetWidth(), GetWindow().GetHeight());
        CreateFramebuffers();
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

        CleanupSwapchain();
        
        vkDestroyBuffer(VulkanDevice::Get().GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(VulkanDevice::Get().GetDevice(), m_VertexBufferMemory, nullptr);
        vkDestroyBuffer(VulkanDevice::Get().GetDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(VulkanDevice::Get().GetDevice(), m_IndexBufferMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(VulkanDevice::Get().GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(VulkanDevice::Get().GetDevice(), m_InFlightFences[i], nullptr);
        }

        Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(GetWindow().GetSwapChain());
        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
            vkDestroySemaphore(VulkanDevice::Get().GetDevice(), m_RenderFinishedSemaphores[i], nullptr);

        vkDestroyCommandPool(VulkanDevice::Get().GetDevice(), m_CommandPool, nullptr);
    }
};

Application* Paradox::CreateApplication()
{
    WindowCreateProperties createProps;
    createProps.Title = "Sandbox";
    createProps.Width = 800;
    createProps.Height = 600;
    return new SandboxApp(createProps);
}