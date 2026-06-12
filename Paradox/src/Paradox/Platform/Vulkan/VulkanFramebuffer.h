#pragma once

#include "Paradox/Renderer/Framebuffer.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"

namespace Paradox
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferProperties& props);
		~VulkanFramebuffer();

		void OnResize(uint32_t width, uint32_t height) override;

		FramebufferProperties& GetProperties() override { return m_Props; }
		const void* GetImageID() override { return m_DescriptorSet; };

		VkFramebuffer GetFramebuffer() { return m_Framebuffer; }
		Shared<RenderPass> GetRenderPass() { return m_RenderPass; }

	private:
		FramebufferProperties m_Props;
		VkImage m_Image;
		VkImageView m_ImageView;
		VkDeviceMemory m_ImageMemory;
		Shared<RenderPass> m_RenderPass;
		VkFramebuffer m_Framebuffer;

		VkDescriptorSet m_DescriptorSet;
	};
}