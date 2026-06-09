#pragma once

#include "Paradox/Renderer/Pipeline.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"

namespace Paradox
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineProperties& props);
		~VulkanPipeline();

		PipelineProperties& GetProperties() override { return m_Properties; }

		VkPipeline GetPipeline() { return m_GraphicsPipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }

	private:
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

		VkFormat GetVulkanFormat(VertexBufferDataType type);
		VkCullModeFlags GetVulkanCullMode(CullMode cullMode);

	private:
		PipelineProperties m_Properties;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}