#include "pxpch.h"
#include "SwapChain.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#include "Paradox/Platform/OpenGL/OpenGLSwapChain.h"

namespace Paradox
{
	Shared<SwapChain> SwapChain::Create()
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanSwapChain>();
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLSwapChain>();
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}