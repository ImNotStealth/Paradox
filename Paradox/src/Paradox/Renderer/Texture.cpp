#include "pxpch.h"
#include "Texture.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanTexture2D.h"
#endif
#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLTexture2D.h"
#endif

namespace Paradox
{
	Shared<Texture2D> Texture2D::Create(const std::string& debugName, const std::filesystem::path& filePath)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanTexture2D>(debugName, filePath);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLTexture2D>(debugName, filePath);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}

	Shared<Texture2D> Texture2D::Create(const TextureProperties& props, const std::filesystem::path& filePath)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanTexture2D>(props, filePath);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLTexture2D>(props, filePath);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}
}