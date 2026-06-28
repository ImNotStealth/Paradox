#pragma once

#include "Paradox/Renderer/Framebuffer.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"
#include "Paradox/Platform/Vulkan/VulkanRenderPass.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"

namespace Paradox
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferProperties& props);
		~VulkanFramebuffer();

		void OnResize(uint32_t width, uint32_t height) override;

		FramebufferProperties& GetProperties() override { return m_Props; }
		uint64_t GetImageID() override { return (uint64_t)m_DescriptorSet; };
		Shared<Image> GetAttachmentImage(uint32_t index) override { return m_Attachments[index]; }

		VkFramebuffer GetFramebuffer() { return m_Framebuffer; }
		Shared<RenderPass> GetRenderPass() { return m_RenderPass; }

	private:
		FramebufferProperties m_Props;
		std::vector<Shared<Image>> m_Attachments;
		Shared<RenderPass> m_RenderPass = nullptr;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
	};
}