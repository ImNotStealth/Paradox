#include "pxpch.h"
#include "OpenGLContext.h"

#include "Paradox/Core/Application.h"

#ifndef PX_PLATFORM_PSVITA
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <vitaGL.h>
#endif

namespace Paradox
{
	void OpenGLContext::Init()
	{
#ifndef PX_PLATFORM_PSVITA
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		glfwMakeContextCurrent(window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");

		std::string rendererName = std::string((const char*)glGetString(GL_RENDERER));
		PX_CORE_INFO("Selected Device: {0}", rendererName);

		glfwSwapInterval(0);
#else
		vglInitExtended(0, 960, 544, 0x1000000, SCE_GXM_MULTISAMPLE_NONE);
		std::string shaderVersion = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
		PX_CORE_INFO("Shader Version: {0}", shaderVersion);
#endif
	}
}