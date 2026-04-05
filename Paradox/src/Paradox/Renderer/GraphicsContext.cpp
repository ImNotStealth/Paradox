#include "pxpch.h"
#include "GraphicsContext.h"

#include "Paradox/Platform/Vulkan/VulkanContext.h"

namespace Paradox
{
	Unique<GraphicsContext> GraphicsContext::Create()
	{
		return CreateUnique<VulkanContext>();
	}
}
