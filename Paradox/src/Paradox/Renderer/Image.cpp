#include "pxpch.h"
#include "Image.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_VULKAN
#include "Paradox/Platform/Vulkan/VulkanImage.h"
#endif
#ifdef PX_INCLUDE_OPENGL
#include "Paradox/Platform/OpenGL/OpenGLImage.h"
#endif

namespace Paradox
{
	Shared<Image> Image::Create(const ImageProperties& props)
	{
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateShared<VulkanImage>(props);
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateShared<OpenGLImage>(props);
#endif
		default:
			PX_CORE_ASSERT(false, "Invalid Graphics API.");
			return nullptr;
		}
	}

	std::string Image::ImageFormatToString(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::SRGBA: return "SRGBA";
		case ImageFormat::RGBA: return "RGBA";
		case ImageFormat::BGRA: return "BGRA";
		case ImageFormat::Depth32F: return "Depth32F";
		}
		PX_CORE_ASSERT(false, "Unknown ImageFormat");
		return "";
	}

	ImageFormat Image::StringToImageFormat(const std::string& str)
	{
		if (str == "SRGBA") return ImageFormat::SRGBA;
		if (str == "RGBA") return ImageFormat::RGBA;
		if (str == "BGRA") return ImageFormat::BGRA;
		if (str == "Depth32F") return ImageFormat::Depth32F;

		PX_CORE_ASSERT(false, "Invalid ImageFormat");
		return ImageFormat::RGBA;
	}

	namespace ImageUtils
	{
		bool IsDepthFormat(ImageFormat format)
		{
			return format == ImageFormat::Depth32F;
		}
	}
}
