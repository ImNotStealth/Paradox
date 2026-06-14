#include "pxpch.h"
#include "Framebuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanFramebuffer.h"
#endif

#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLFramebuffer.h"
#endif

namespace Paradox
{
	Shared<Framebuffer> Framebuffer::Create(const FramebufferProperties& props)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanFramebuffer>(props);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLFramebuffer>(props);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}