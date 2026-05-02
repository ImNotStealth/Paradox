#include "pxpch.h"
#include "Framebuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanFramebuffer.h"

namespace Paradox
{
	Shared<Framebuffer> Framebuffer::Create(const FramebufferProperties& props)
	{
		return nullptr;
		//switch (GraphicsContext::GetGraphicsAPI())
		//{
		//case GraphicsAPIType::Vulkan:
		//	return CreateShared<VulkanFramebuffer>(props);
		//default:
		//	PX_CORE_ASSERT(false, "Invalid Graphics API.");
		//	return nullptr;
		//}
	}
}