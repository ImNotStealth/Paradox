#include "pxpch.h"
#include "VulkanPipeline.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanRenderPass.h"

namespace Paradox
{
	VulkanPipeline::VulkanPipeline(const PipelineProperties& props)
		: m_Properties(props)
	{
        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
        vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkVertexInputBindingDescription bindingDesc;
        bindingDesc.binding = 0;
        bindingDesc.stride = props.layout.GetStride();
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        auto attributeDesc = GetAttributeDescriptions();
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
        rasterizationStateCreateInfo.polygonMode = props.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = props.wireframeWidth;
        rasterizationStateCreateInfo.cullMode = GetVulkanCullMode(props.cullMode);
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

        VkResult layoutResult = vkCreatePipelineLayout(VulkanDevice::Get().GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
        PX_ASSERT(layoutResult == VK_SUCCESS, "Failed to create Pipeline Layout.");

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = props.shader->GetStageCount();
        graphicsPipelineCreateInfo.pStages = props.shader->GetShaderStages();
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = m_PipelineLayout;

        Shared<VulkanRenderPass> renderPass = std::static_pointer_cast<VulkanRenderPass>(props.renderPass);
        graphicsPipelineCreateInfo.renderPass = renderPass->GetRenderPass();
        graphicsPipelineCreateInfo.subpass = 0;

        VkResult graphicsResult = vkCreateGraphicsPipelines(VulkanDevice::Get().GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_GraphicsPipeline);
        PX_ASSERT(graphicsResult == VK_SUCCESS, "Failed to create Graphics Pipeline.");

        PX_CORE_TRACE("Pipeline Created: {0}", m_Properties.debugName);
	}

	VulkanPipeline::~VulkanPipeline()
	{
        PX_CORE_TRACE("Pipeline Destroyed: {0}", m_Properties.debugName);
        vkDestroyPipeline(VulkanDevice::Get().GetDevice(), m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(VulkanDevice::Get().GetDevice(), m_PipelineLayout, nullptr);
	}

    std::vector<VkVertexInputAttributeDescription> VulkanPipeline::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescs(m_Properties.layout.GetElements().size());

        uint32_t i = 0;
        for (const VertexBufferElement& element : m_Properties.layout.GetElements())
        {
            VkVertexInputAttributeDescription& attrib = attributeDescs[i];
            attrib.binding = 0;
            attrib.location = i;
            attrib.format = GetVulkanFormat(element.type);
			attrib.offset = element.offset;
            i++;
        }

        return attributeDescs;
    }

    VkFormat VulkanPipeline::GetVulkanFormat(VertexBufferDataType type)
    {
        switch (type)
        {
        case VertexBufferDataType::Float: return VK_FORMAT_R32_SFLOAT;
        case VertexBufferDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
        case VertexBufferDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexBufferDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
		PX_CORE_ASSERT(false, "Invalid VertexBufferDataType.");
        return VK_FORMAT_UNDEFINED;
    }

    VkCullModeFlags VulkanPipeline::GetVulkanCullMode(CullMode cullMode)
    {
        switch (cullMode)
        {
        case CullMode::None: return VK_CULL_MODE_NONE;
        case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
        }
        PX_CORE_ASSERT(false, "Invalid CullMode.");
        return VK_CULL_MODE_NONE;
    }
}
