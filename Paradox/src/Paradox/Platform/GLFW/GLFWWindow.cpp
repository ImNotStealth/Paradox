#include "pxpch.h"
#include "GLFWWindow.h"

#include "Paradox/Events/ApplicationEvents.h"
#include <vulkan/vulkan.h>

namespace Paradox
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		PX_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	GLFWWindow::GLFWWindow(const WindowCreateProperties& props)
	{
		m_WindowData.Title = props.Title;
		m_WindowData.Width = props.Width;
		m_WindowData.Height = props.Height;

		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			PX_CORE_ASSERT(success, "Failed to initialize GLFW.");
			glfwSetErrorCallback(GLFWErrorCallback);
		}
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // This is vulkan only

		m_Window = glfwCreateWindow(m_WindowData.Width, m_WindowData.Height, m_WindowData.Title.c_str(), nullptr, nullptr);
		s_GLFWWindowCount++;

		glfwSetWindowUserPointer(m_Window, &m_WindowData);

		AssignCallbacks();

		PX_CORE_INFO("Created Window {0} {1}x{2}", m_WindowData.Title, m_WindowData.Width, m_WindowData.Height);
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;

		--s_GLFWWindowCount;
		PX_CORE_INFO("Destroyed Window {0}", m_WindowData.Title);

		if (s_GLFWWindowCount == 0)
		{
			PX_CORE_INFO("Terminated GLFW.");
			glfwTerminate();
		}
	}

	void GLFWWindow::WaitEvents()
	{
		glfwWaitEvents();
	}

	std::vector<const char*> GLFWWindow::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		return extensions;
	}

	void GLFWWindow::CreateSurface(void* instance, void* surface)
	{
		VkResult result = glfwCreateWindowSurface((VkInstance)instance, m_Window, nullptr, (VkSurfaceKHR*)surface);
		PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Surface");
	}

	void GLFWWindow::PollEvents()
	{
		glfwPollEvents();
	}

	void GLFWWindow::AssignCallbacks()
	{
		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			GLFWWindowData& data = *(GLFWWindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			GLFWWindowData& data = *(GLFWWindowData*)glfwGetWindowUserPointer(window);
			
			WindowCloseEvent event;
			data.EventCallback(event);
		});
	}
}