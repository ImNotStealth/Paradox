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

		void Resize(uint32_t width, uint32_t height) override;

		FramebufferProperties& GetProperties() override { return m_Props; }
		Shared<Image> GetAttachmentImage(uint32_t index) override { PX_CORE_ASSERT(index < m_Attachments.size()); return m_Attachments[index]; }

		VkFramebuffer GetFramebuffer() { return m_Framebuffer; }
		Shared<VulkanRenderPass> GetRenderPass() { return m_RenderPass; }

	private:
		FramebufferProperties m_Props;
		std::vector<Shared<Image>> m_Attachments;
		Shared<VulkanRenderPass> m_RenderPass = nullptr;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};
}