#include "pxpch.h"
#include "IndexBuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanIndexBuffer.h"
#endif
#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLIndexBuffer.h"
#endif

namespace Paradox
{
	Shared<IndexBuffer> IndexBuffer::Create(uint32_t count, IndexBufferUsage usage)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanIndexBuffer>(count, usage);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLIndexBuffer>(count, usage);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}

	Shared<IndexBuffer> IndexBuffer::Create(const void* data, uint32_t count, IndexBufferUsage usage)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanIndexBuffer>(data, count, usage);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLIndexBuffer>(data, count, usage);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}