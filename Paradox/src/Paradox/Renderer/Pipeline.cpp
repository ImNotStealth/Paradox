#include "pxpch.h"
#include "Pipeline.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanPipeline.h"
#endif

#include "Paradox/Platform/OpenGL/OpenGLPipeline.h"

namespace Paradox
{
	Shared<Pipeline> Pipeline::Create(const PipelineProperties& props)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanPipeline>(props);
#endif
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLPipeline>(props);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}