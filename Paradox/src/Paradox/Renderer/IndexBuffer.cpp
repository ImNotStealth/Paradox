#include "pxpch.h"
#include "IndexBuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanIndexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLIndexBuffer.h"

namespace Paradox
{
	Shared<IndexBuffer> IndexBuffer::Create(uint32_t count, IndexBufferUsage usage)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanIndexBuffer>(count, usage);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLIndexBuffer>(count, usage);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}

	Shared<IndexBuffer> IndexBuffer::Create(const void* data, uint32_t count, IndexBufferUsage usage)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanIndexBuffer>(data, count, usage);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLIndexBuffer>(data, count, usage);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}