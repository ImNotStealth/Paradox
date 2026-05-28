#include "pxpch.h"
#include "SwapChain.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#endif
#include "Paradox/Platform/OpenGL/OpenGLSwapChain.h"

namespace Paradox
{
	Shared<SwapChain> SwapChain::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanSwapChain>();
#endif
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLSwapChain>();
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}