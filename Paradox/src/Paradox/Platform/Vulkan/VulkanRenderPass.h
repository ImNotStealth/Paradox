#pragma once

#include "Paradox/Platform/Vulkan/Vulkan.h"
#include "Paradox/Renderer/Image.h"

namespace Paradox
{
	struct RenderPassProperties
	{
		bool clearColor = true;
		bool swapchainTarget = false;
		std::vector<ImageFormat> attachments = { ImageFormat::RGBA };
		std::string debugName;
	};

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(const RenderPassProperties& props);
		~VulkanRenderPass();

		RenderPassProperties& GetProperties() { return m_Properties; }

		VkRenderPass& GetRenderPass() { return m_RenderPass; }

	private:
		RenderPassProperties m_Properties;
		VkRenderPass m_RenderPass;
	};
}