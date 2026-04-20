#include "pxpch.h"
#include "RenderPass.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanRenderPass.h"

namespace Paradox
{
	Shared<RenderPass> RenderPass::Create(const RenderPassProperties& props)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanRenderPass>(props);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}