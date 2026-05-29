#include "pxpch.h"

#include "Window.h"

#if defined(PX_PLATFORM_WINDOWS) || defined(PX_PLATFORM_LINUX)
	#define USE_GLFW_WINDOW
#endif

#ifdef USE_GLFW_WINDOW
#include "Paradox/Platform/GLFW/GLFWWindow.h"
#endif

#ifdef PX_PLATFORM_PSVITA
#include "Paradox/Platform/Vita/VitaWindow.h"
#endif

namespace Paradox
{
	Unique<Window> Window::Create(const WindowCreateProperties& props)
	{
#ifdef USE_GLFW_WINDOW
		return CreateUnique<GLFWWindow>(props);
#elif defined(PX_PLATFORM_PSVITA)
		return CreateUnique<VitaWindow>(props);
#else
		PX_CORE_ASSERT(false, "Unknown Platform.");
#endif
	}
}