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

	Shared<Texture2D> Texture2D::CreateFromImage(const TextureProperties& props, Shared<Image> image)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanTexture2D>(props, image);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLTexture2D>(props, image);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}

	std::string Texture::TextureWrapToString(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat: return "Repeat";
		case TextureWrap::MirroredRepeat: return "MirroredRepeat";
		case TextureWrap::ClampToBorder: return "ClampToBorder";
		case TextureWrap::ClampToEdge: return "ClampToEdge";
		}
		PX_CORE_ASSERT(false, "Unknown TextureWrap");
		return "";
	}

	TextureWrap Texture::StringToTextureWrap(const std::string& str)
	{
		if (str == "Repeat") return TextureWrap::Repeat;
		if (str == "MirroredRepeat") return TextureWrap::MirroredRepeat;
		if (str == "ClampToBorder") return TextureWrap::ClampToBorder;
		if (str == "ClampToEdge") return TextureWrap::ClampToEdge;

		PX_CORE_ASSERT(false, "Invalid TextureWrap");
		return TextureWrap::Repeat;
	}

	std::string Texture::TextureFilterToString(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest: return "Nearest";
		case TextureFilter::Linear: return "Linear";
		}
		PX_CORE_ASSERT(false, "Unknown TextureFilter");
		return "";
	}

	TextureFilter Texture::StringToTextureFilter(const std::string& str)
	{
		if (str == "Nearest") return TextureFilter::Nearest;
		if (str == "Linear") return TextureFilter::Linear;

		PX_CORE_ASSERT(false, "Invalid TextureFilter");
		return TextureFilter::Nearest;
	}
}