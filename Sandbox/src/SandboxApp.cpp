#include <Paradox.h>

#include <Paradox/Events/ApplicationEvents.h>
#include <Paradox/Renderer/Shader.h>
#include <Paradox/Platform/Vulkan/VulkanContext.h>
#include <Paradox/Platform/Vulkan/VulkanDevice.h>
#include <Paradox/Platform/Vulkan/VulkanSwapChain.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <fstream>
#include <glm/glm.hpp>

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef PX_DEBUG
    const bool validationLayersEnabled = true;
#else
    const bool validationLayersEnabled = false;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (!func)
        return VK_ERROR_EXTENSION_NOT_PRESENT;

    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
        func(instance, debugMessenger, pAllocator);
}

class SandboxApp : public Paradox::Application
{
public:
    SandboxApp(const Paradox::WindowCreateProperties& windowProps)
        : Application(windowProps) {}

    void Run() override
    {
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    VkRenderPass m_RenderPass = {};
    VkPipelineLayout m_PipelineLayout = {};
    VkPipeline m_GraphicsPipeline = {};
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
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    bool OnResize(Paradox::WindowResizeEvent& e) override
    {
        m_FramebufferResized = true;
        return false;
    }

    void CreateRenderPass()
    {
        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapchain->GetColorFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;

        VkSubpassDependency subpassDependency = {};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &subpassDependency;

        VkResult result = vkCreateRenderPass(Paradox::VulkanDevice::Get().GetDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass);
        PX_ASSERT(result == VK_SUCCESS, "Failed to create RenderPass.");
    }

    void CreateGraphicsPipeline()
    {
        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
        vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        VkVertexInputBindingDescription bindingDesc = Vertex::GetBindingDescription();
        auto attributeDesc = Vertex::GetAttributeDescriptions();
        vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDesc;
        vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDesc.data();
        vertexInputCreateInfo.vertexAttributeDescriptionCount = (size_t)attributeDesc.size();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
        inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = 1.f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
        colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendCreateInfo.attachmentCount = 1;
        colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkResult layoutResult = vkCreatePipelineLayout(Paradox::VulkanDevice::Get().GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
        PX_ASSERT(layoutResult == VK_SUCCESS, "Failed to create Pipeline Layout.");

        Paradox::Shader shader = Paradox::Shader("TestShader", Paradox::VulkanDevice::Get().GetDevice(), "shaders/compiled/shader.vert.spv", "shaders/compiled/shader.frag.spv");

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = shader.GetStageCount();
        graphicsPipelineCreateInfo.pStages = shader.GetShaderStages();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = m_PipelineLayout;
        graphicsPipelineCreateInfo.renderPass = m_RenderPass;
        graphicsPipelineCreateInfo.subpass = 0;

        VkResult graphicsResult = vkCreateGraphicsPipelines(Paradox::VulkanDevice::Get().GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_GraphicsPipeline);
        PX_ASSERT(graphicsResult == VK_SUCCESS, "Failed to create Graphics Pipeline.");
    }

    struct Vertex
    {
        glm::vec2 m_Pos;
        glm::vec3 m_Color;

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDesc;
            bindingDesc.binding = 0;
            bindingDesc.stride = sizeof(Vertex);
            bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDesc;
        }

        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescs;

            attributeDescs[0].binding = 0;
            attributeDescs[0].location = 0;
            attributeDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescs[0].offset = offsetof(Vertex, m_Pos);

            attributeDescs[1].binding = 0;
            attributeDescs[1].location = 1;
            attributeDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescs[1].offset = offsetof(Vertex, m_Color);

            return attributeDescs;
        }
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
        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();

        m_SwapchainFramebuffers.resize(swapchain->GetImageCount());
        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
        {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = m_RenderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = &swapchain->GetImages()[i].ImageView;
            framebufferCreateInfo.width = swapchain->GetExtent().width;
            framebufferCreateInfo.height = swapchain->GetExtent().height;
            framebufferCreateInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(Paradox::VulkanDevice::Get().GetDevice(), &framebufferCreateInfo, nullptr, &m_SwapchainFramebuffers[i]);
            PX_ASSERT(result == VK_SUCCESS, "Failed to create Framebuffer.");
        }
    }

    void CreateCommandPool()
    {
        Paradox::VulkanDevice::QueueFamilyIndices familyIndices = Paradox::VulkanDevice::Get().GetQueueFamilyIndices();
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = familyIndices.GraphicsFamily;

        VkResult result = vkCreateCommandPool(Paradox::VulkanDevice::Get().GetDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool);
        PX_ASSERT(result == VK_SUCCESS, "Failed to create Command Pool.");
    }

    void CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        
        VkBuffer stagingBuffer = {};
        VkDeviceMemory stagingBufferMemory = {};
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(Paradox::VulkanDevice::Get().GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(Paradox::VulkanDevice::Get().GetDevice(), stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
        CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(Paradox::VulkanDevice::Get().GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(Paradox::VulkanDevice::Get().GetDevice(), stagingBufferMemory, nullptr);
    }

    void CreateIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer = {};
        VkDeviceMemory stagingBufferMemory = {};
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(Paradox::VulkanDevice::Get().GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(Paradox::VulkanDevice::Get().GetDevice(), stagingBufferMemory);

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
        CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(Paradox::VulkanDevice::Get().GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(Paradox::VulkanDevice::Get().GetDevice(), stagingBufferMemory, nullptr);
    }

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmdBuffer = {};
        vkAllocateCommandBuffers(Paradox::VulkanDevice::Get().GetDevice(), &allocInfo, &cmdBuffer);

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
        vkQueueSubmit(Paradox::VulkanDevice::Get().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(Paradox::VulkanDevice::Get().GetGraphicsQueue());
        vkFreeCommandBuffers(Paradox::VulkanDevice::Get().GetDevice(), m_CommandPool, 1, &cmdBuffer);
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties = {};
        vkGetPhysicalDeviceMemoryProperties(Paradox::VulkanDevice::Get().GetPhysicalDevice(), &memProperties);

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

        VkResult bufferResult = vkCreateBuffer(Paradox::VulkanDevice::Get().GetDevice(), &bufferCreateInfo, nullptr, &buffer);
        PX_ASSERT(bufferResult == VK_SUCCESS, "Failed to create Buffer.");

        VkMemoryRequirements memRequirements = {};
        vkGetBufferMemoryRequirements(Paradox::VulkanDevice::Get().GetDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, propFlags);

        VkResult allocateResult = vkAllocateMemory(Paradox::VulkanDevice::Get().GetDevice(), &allocInfo, nullptr, &bufferMemory);
        PX_ASSERT(allocateResult == VK_SUCCESS, "Failed to allocate Buffer memory.");

        vkBindBufferMemory(Paradox::VulkanDevice::Get().GetDevice(), buffer, bufferMemory, 0);
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

        VkResult result = vkAllocateCommandBuffers(Paradox::VulkanDevice::Get().GetDevice(), &allocInfo, m_CommandBuffers.data());
        PX_ASSERT(result == VK_SUCCESS, "Failed to allocate Command Buffers.");
    }
    
    void CreateSyncObjects()
    {
        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();

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
            bool createSuccess = vkCreateSemaphore(Paradox::VulkanDevice::Get().GetDevice(), &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) == VK_SUCCESS &&
                vkCreateFence(Paradox::VulkanDevice::Get().GetDevice(), &fenceCreateInfo, nullptr, &m_InFlightFences[i]) == VK_SUCCESS;
            PX_ASSERT(createSuccess, "Failed to create sync objects for a frame.");
        }

        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
        {
            VkResult createResult = vkCreateSemaphore(Paradox::VulkanDevice::Get().GetDevice(), &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
            PX_ASSERT(createResult == VK_SUCCESS, "Failed to create RenderFinished semaphore.");
        }
    }

    void RecordCommandBuffer(const VkCommandBuffer& cmdBuffer, uint32_t imageIndex)
    {
        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();

        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkResult beginCmdBufferResult = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
        PX_ASSERT(beginCmdBufferResult == VK_SUCCESS, "Failed to begin recording Command Buffer.");

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_RenderPass;
        renderPassBeginInfo.framebuffer = m_SwapchainFramebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = swapchain->GetExtent();

        VkClearValue clearColor = { 0.f, 0.f, 0.f, 1.f };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

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
            vkDestroyFramebuffer(Paradox::VulkanDevice::Get().GetDevice(), m_SwapchainFramebuffers[i], nullptr);
    }

    void RecreateSwapchain()
    {
        while (GetWindow().GetWidth() == 0 || GetWindow().GetHeight() == 0)
        {
            GetWindow().WaitEvents();
        }

        GetWindow().GetGraphicsContext()->WaitIdle();
        CleanupSwapchain();
        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();
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

        vkDeviceWaitIdle(Paradox::VulkanDevice::Get().GetDevice());
    }

    void DrawFrame()
    {
        vkWaitForFences(Paradox::VulkanDevice::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();

        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(Paradox::VulkanDevice::Get().GetDevice(), swapchain->GetSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return;
        }
        else
            PX_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image.");

        vkResetFences(Paradox::VulkanDevice::Get().GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

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

        VkResult queueResult = vkQueueSubmit(Paradox::VulkanDevice::Get().GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
        PX_ASSERT(queueResult == VK_SUCCESS, "Failed to submit to draw Command Buffer.");
    
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = { swapchain->GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(Paradox::VulkanDevice::Get().GetPresentQueue(), &presentInfo);
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
        
        vkDestroyBuffer(Paradox::VulkanDevice::Get().GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(Paradox::VulkanDevice::Get().GetDevice(), m_VertexBufferMemory, nullptr);
        vkDestroyBuffer(Paradox::VulkanDevice::Get().GetDevice(), m_IndexBuffer, nullptr);
        vkFreeMemory(Paradox::VulkanDevice::Get().GetDevice(), m_IndexBufferMemory, nullptr);

        vkDestroyPipeline(Paradox::VulkanDevice::Get().GetDevice(), m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(Paradox::VulkanDevice::Get().GetDevice(), m_PipelineLayout, nullptr);

        vkDestroyRenderPass(Paradox::VulkanDevice::Get().GetDevice(), m_RenderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(Paradox::VulkanDevice::Get().GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(Paradox::VulkanDevice::Get().GetDevice(), m_InFlightFences[i], nullptr);
        }

        Paradox::Shared<Paradox::VulkanSwapChain>& swapchain = (Paradox::Shared<Paradox::VulkanSwapChain>&)GetWindow().GetSwapChain();
        for (size_t i = 0; i < swapchain->GetImageCount(); i++)
            vkDestroySemaphore(Paradox::VulkanDevice::Get().GetDevice(), m_RenderFinishedSemaphores[i], nullptr);

        vkDestroyCommandPool(Paradox::VulkanDevice::Get().GetDevice(), m_CommandPool, nullptr);
    }
};

Paradox::Application* Paradox::CreateApplication()
{
    WindowCreateProperties createProps;
    createProps.Title = "Sandbox";
    createProps.Width = 800;
    createProps.Height = 600;
    return new SandboxApp(createProps);
}