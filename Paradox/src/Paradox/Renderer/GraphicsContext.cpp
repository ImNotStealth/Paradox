#include "pxpch.h"
#include "GraphicsContext.h"

#include "Paradox/Platform/Vulkan/VulkanContext.h"

namespace Paradox
{
	GraphicsAPIType GraphicsContext::s_GraphicsAPI = GraphicsAPIType::None;

	Shared<GraphicsContext> GraphicsContext::Create()
	{
		switch (s_GraphicsAPI)
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanContext>();
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}
