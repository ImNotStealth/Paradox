#include "pxpch.h"
#include "Paradox/Core/Input.h"

#include "Paradox/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Paradox
{
	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		int state = glfwGetKey(window, (int)keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
}