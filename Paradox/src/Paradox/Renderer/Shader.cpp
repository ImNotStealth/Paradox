#include "pxpch.h"
#include "Shader.h"

#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Platform/Vulkan/VulkanShader.h"

namespace Paradox
{
    Shared<Shader> Shader::Create(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
    {
		switch (GraphicsContext::GetGraphicsAPI())
		{
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanShader>(name, vertFilePath, fragFilePath);
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
    }
}