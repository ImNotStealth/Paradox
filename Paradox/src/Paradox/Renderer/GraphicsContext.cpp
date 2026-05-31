#include "pxpch.h"
#include "GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanContext.h"
#endif
#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLContext.h"
#endif

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
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLContext>();
#endif
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
