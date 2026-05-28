#include "pxpch.h"
#include "UniformBuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanUniformBuffer.h"
#endif
#include "Paradox/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Paradox
{
	Shared<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanUniformBuffer>(size);
#endif
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLUniformBuffer>(size);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}