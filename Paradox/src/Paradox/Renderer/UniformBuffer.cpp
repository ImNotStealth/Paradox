#include "pxpch.h"
#include "UniformBuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Paradox
{
	Shared<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanUniformBuffer>(size);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLUniformBuffer>(size);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}