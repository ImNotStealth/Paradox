#include "pxpch.h"
#include "RendererAPI.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanRendererAPI.h"
#include "Paradox/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Paradox
{
	Unique<RendererAPI> RendererAPI::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateUnique<VulkanRendererAPI>();
		case GraphicsAPIType::OpenGL:
			return CreateUnique<OpenGLRendererAPI>();
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}