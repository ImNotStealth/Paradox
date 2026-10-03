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

	bool Input::IsMousePressed(KeyCode keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		int state = glfwGetMouseButton(window, (int)keyCode);
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePos()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		double x = 0, y = 0;
		glfwGetCursorPos(window, &x, &y);
		return { (float)x, (float)y };
	}

	void Input::SetMouseCursorMode(CursorMode mode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetHandle());
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
	}
}