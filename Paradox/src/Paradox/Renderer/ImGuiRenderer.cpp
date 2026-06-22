#include "pxpch.h"
#include "ImGuiRenderer.h"

#include "Paradox/Renderer/GraphicsContext.h"

#ifdef PX_INCLUDE_IMGUI
	#ifdef PX_INCLUDE_VULKAN
	#include "Paradox/Platform/Vulkan/VulkanImGuiRenderer.h"
	#endif
	#ifdef PX_INCLUDE_OPENGL
	#include "Paradox/Platform/OpenGL/OpenGLImGuiRenderer.h"
	#endif
#endif

namespace Paradox
{
	Unique<ImGuiRenderer> ImGuiRenderer::Create()
	{
#ifdef PX_INCLUDE_IMGUI
		switch (GraphicsContext::GetGraphicsAPI())
		{
#ifdef PX_INCLUDE_VULKAN
		case GraphicsAPIType::Vulkan:
			return CreateUnique<VulkanImGuiRenderer>();
#endif
#ifdef PX_INCLUDE_OPENGL
		case GraphicsAPIType::OpenGL:
			return CreateUnique<OpenGLImGuiRenderer>();
#endif
		default:
			PX_CORE_ERROR("ImGui is enabled but no Renderer has been found. ImGui will be unavailable.");
		}
#endif
		return nullptr;
	}
}