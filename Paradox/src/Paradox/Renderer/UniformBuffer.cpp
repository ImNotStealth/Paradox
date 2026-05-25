#include "pxpch.h"
#include "UniformBuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBuffer.h"

namespace Paradox
{
	Shared<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanUniformBuffer>(size);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}