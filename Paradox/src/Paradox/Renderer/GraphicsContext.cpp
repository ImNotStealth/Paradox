#include "pxpch.h"
#include "GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanContext.h"
#endif
#include "Paradox/Platform/OpenGL/OpenGLContext.h"

namespace Paradox
{
	GraphicsAPIType GraphicsContext::s_GraphicsAPI = GraphicsAPIType::None;

	Shared<GraphicsContext> GraphicsContext::Create()
	{
		switch (s_GraphicsAPI)
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanContext>();
#endif
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLContext>();
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
	std::string GraphicsContext::GraphicsAPIToString(GraphicsAPIType api)
	{
		switch (api)
		{
		case GraphicsAPIType::Vulkan: return "Vulkan";
		case GraphicsAPIType::OpenGL: return "OpenGL";
		}
		PX_CORE_ASSERT(false, "Invalid Graphics API.");
		return "None";
	}
}
