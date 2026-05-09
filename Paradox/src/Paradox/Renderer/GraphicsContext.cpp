#include "pxpch.h"
#include "GraphicsContext.h"

#include "Paradox/Platform/Vulkan/VulkanContext.h"
#include "Paradox/Platform/OpenGL/OpenGLContext.h"

namespace Paradox
{
	GraphicsAPIType GraphicsContext::s_GraphicsAPI = GraphicsAPIType::None;

	Shared<GraphicsContext> GraphicsContext::Create()
	{
		switch (s_GraphicsAPI)
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanContext>();
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
