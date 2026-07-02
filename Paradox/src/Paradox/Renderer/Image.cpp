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

	namespace ImageUtils
	{
		bool IsDepthFormat(ImageFormat format)
		{
			return format == ImageFormat::Depth32F;
		}
	}
}
