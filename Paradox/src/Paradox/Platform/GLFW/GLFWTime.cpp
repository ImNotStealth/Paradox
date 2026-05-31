#include "pxpch.h"
#include "Paradox/Core/Time.h"

#include <GLFW/glfw3.h>

namespace Paradox
{
	float Time::GetTime()
	{
		return glfwGetTime();
	}
}