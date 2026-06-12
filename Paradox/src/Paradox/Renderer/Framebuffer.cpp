#include "pxpch.h"
#include "Framebuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanFramebuffer.h"
#endif

namespace Paradox
{
	Shared<Framebuffer> Framebuffer::Create(const FramebufferProperties& props)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanFramebuffer>(props);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}