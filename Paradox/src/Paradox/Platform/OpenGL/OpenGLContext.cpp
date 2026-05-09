#include "pxpch.h"
#include "OpenGLContext.h"

#include "Paradox/Core/Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Paradox
{
	void OpenGLContext::Init()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		glfwMakeContextCurrent(window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");

		std::string rendererName = std::string((const char*)glGetString(GL_RENDERER));
		PX_CORE_INFO("Selected Device: {0}", rendererName);

		glfwSwapInterval(0);
	}
}