#include "pxpch.h"
#include "UniformBufferSet.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanUniformBufferSet.h"
#endif

#include "Paradox/Platform/OpenGL/OpenGLUniformBufferSet.h"

namespace Paradox
{
	Shared<UniformBufferSet> UniformBufferSet::Create(uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanUniformBufferSet>(size);
#endif
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLUniformBufferSet>(size);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}