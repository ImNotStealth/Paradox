#include "pxpch.h"
#include "Pipeline.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanPipeline.h"
#include "Paradox/Platform/OpenGL/OpenGLPipeline.h"

namespace Paradox
{
	Shared<Pipeline> Pipeline::Create(const PipelineProperties& props)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanPipeline>(props);
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLPipeline>(props);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}