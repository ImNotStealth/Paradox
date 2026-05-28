#include "pxpch.h"
#include "Shader.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanShader.h"
#endif

#include "Paradox/Platform/OpenGL/OpenGLShader.h"

namespace Paradox
{
    Shared<Shader> Shader::Create(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
    {
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanShader>(name, vertFilePath, fragFilePath);
#endif
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLShader>(name, vertFilePath, fragFilePath);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
    }
}