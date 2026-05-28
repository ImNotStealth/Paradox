#include "pxpch.h"
#include "RendererAPI.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanRendererAPI.h"
#endif
#include "Paradox/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Paradox
{
	Unique<RendererAPI> RendererAPI::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateUnique<VulkanRendererAPI>();
#endif
		case GraphicsAPIType::OpenGL:
			return CreateUnique<OpenGLRendererAPI>();
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}