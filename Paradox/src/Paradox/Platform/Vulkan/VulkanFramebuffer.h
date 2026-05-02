#pragma once

#include "Paradox/Renderer/Framebuffer.h"

//#include <vulkan/vulkan.h>
//
//namespace Paradox
//{
//	class VulkanFramebuffer : public Framebuffer
//	{
//	public:
//		VulkanFramebuffer(const FramebufferProperties& props);
//		~VulkanFramebuffer();
//
//		void OnResize(uint32_t width, uint32_t height) override;
//		FramebufferProperties& GetProperties() override { return m_Props; }
//
//	private:
//		FramebufferProperties m_Props;
//
//		VkFramebuffer m_Framebuffer;
//	};
//}