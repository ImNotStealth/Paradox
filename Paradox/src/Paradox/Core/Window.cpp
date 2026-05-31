#include "pxpch.h"

#include "Window.h"

#ifdef PX_INCLUDE_GLFW
	#include "Paradox/Platform/GLFW/GLFWWindow.h"
#endif

#ifdef PX_INCLUDE_VITASDK
	#include "Paradox/Platform/Vita/VitaWindow.h"
#endif

namespace Paradox
{
	Unique<Window> Window::Create(const WindowCreateProperties& props)
	{
#if defined(PX_PLATFORM_WINDOWS) || defined(PX_PLATFORM_LINUX)
		return CreateUnique<GLFWWindow>(props);
#elif defined(PX_PLATFORM_PSVITA)
		return CreateUnique<VitaWindow>(props);
#else
		PX_CORE_ASSERT(false, "Unknown Platform.");
#endif
	}
}