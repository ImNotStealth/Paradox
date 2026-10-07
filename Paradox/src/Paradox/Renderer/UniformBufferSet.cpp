#include "pxpch.h"
#include "UniformBufferSet.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanUniformBufferSet.h"
#endif
#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLUniformBufferSet.h"
#endif

namespace Paradox
{
	Reference<UniformBufferSet> UniformBufferSet::Create(uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateRef<VulkanUniformBufferSet>(size);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateRef<OpenGLUniformBufferSet>(size);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}