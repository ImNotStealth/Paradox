#include "pxpch.h"
#include "VulkanFramebuffer.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& props)
	{

	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		vkDestroyFramebuffer(VulkanDevice::Get().GetDevice(), m_Framebuffer, nullptr);
	}
}