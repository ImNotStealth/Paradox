#pragma once

#include "Paradox/Renderer/RenderPass.h"

#include <vulkan/vulkan.h>

namespace Paradox
{
	// PARADOX_API is temporary
	class PARADOX_API VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassProperties& props);
		~VulkanRenderPass();

		RenderPassProperties& GetProperties() override { return m_Properties; };

		//TEMPORARY
		VkRenderPass& GetRenderPass() { return m_RenderPass; }

	private:
		RenderPassProperties m_Properties;
		VkRenderPass m_RenderPass;
	};
}