#include "pxpch.h"
#include "UniformBufferSet.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBufferSet.h"
#include "Paradox/Platform/OpenGL/OpenGLUniformBufferSet.h"

namespace Paradox
{
	Shared<UniformBufferSet> UniformBufferSet::Create(uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanUniformBufferSet>(size);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLUniformBufferSet>(size);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}