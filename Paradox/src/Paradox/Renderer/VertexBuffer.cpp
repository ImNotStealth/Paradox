#include "pxpch.h"
#include "VertexBuffer.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLVertexBuffer.h"

namespace Paradox
{
	Shared<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanVertexBuffer>(size, usage);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLVertexBuffer>(size, usage);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
	
	Shared<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, VertexBufferUsage usage)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanVertexBuffer>(data, size, usage);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLVertexBuffer>(data, size, usage);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}