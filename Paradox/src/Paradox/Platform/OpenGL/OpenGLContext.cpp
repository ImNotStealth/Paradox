#include "pxpch.h"
#include "OpenGLContext.h"

#include "Paradox/Core/Application.h"

#ifdef PX_INCLUDE_VITASDK
#include <vitaGL.h>
#endif
#ifdef PX_INCLUDE_GLAD
#include <glad/glad.h>
#endif
#ifdef PX_INCLUDE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace Paradox
{
	void OpenGLContext::Init()
	{
#if defined(PX_PLATFORM_WINDOWS) || defined(PX_PLATFORM_LINUX)
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		glfwMakeContextCurrent(window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");

		glfwSwapInterval(0);
#elif defined(PX_PLATFORM_PSVITA)
		vglInitExtended(0, 960, 544, 0x1000000, SCE_GXM_MULTISAMPLE_NONE);
#endif

		std::string shaderVersion = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		PX_CORE_INFO("Shader Version: {0}", shaderVersion);

		std::string rendererName = std::string((const char*)glGetString(GL_RENDERER));
		PX_CORE_INFO("Selected Device: {0}", rendererName);
	}
}