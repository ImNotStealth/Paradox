#pragma once

#include "Paradox/Renderer/Pipeline.h"

namespace Paradox
{
	// PARADOX_API is temporary
	class PARADOX_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineProperties& props);
		~VulkanPipeline();

		PipelineProperties& GetProperties() override { return m_Properties; }

		VkPipeline GetPipeline() { return m_GraphicsPipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }

	private:
		//TODO: Replace with a way to manually specify a layout
		VkVertexInputBindingDescription GetBindingDescription();
		std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();

	private:
		PipelineProperties m_Properties;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}