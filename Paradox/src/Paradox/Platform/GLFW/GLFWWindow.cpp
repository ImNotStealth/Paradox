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
		m_WindowData.title = props.title;
		m_WindowData.width = props.width;
		m_WindowData.height = props.height;
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;

		--s_GLFWWindowCount;
		PX_CORE_INFO("Destroyed Window {0}", m_WindowData.title);

		if (s_GLFWWindowCount == 0)
		{
			PX_CORE_INFO("Terminated GLFW.");
			glfwTerminate();
		}
	}

	void GLFWWindow::Init()
	{
		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			PX_CORE_ASSERT(success, "Failed to initialize GLFW.");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(m_WindowData.width, m_WindowData.height, m_WindowData.title.c_str(), nullptr, nullptr);
		s_GLFWWindowCount++;

		glfwSetWindowUserPointer(m_Window, &m_WindowData);

		AssignCallbacks();

		m_GraphicsContext = GraphicsContext::Create();
		m_GraphicsContext->Init();

		bool vsyncEnabled = false;
		m_SwapChain = SwapChain::Create();
		m_SwapChain->Init(this);
		m_SwapChain->Create(m_WindowData.width, m_WindowData.height, vsyncEnabled);

		PX_CORE_INFO("Created Window {0} {1}x{2}", m_WindowData.title, m_WindowData.width, m_WindowData.height);
	}

	void GLFWWindow::WaitEvents()
	{
		glfwWaitEvents();
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
			data.width = width;
			data.height = height;
			
			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			GLFWWindowData& data = *(GLFWWindowData*)glfwGetWindowUserPointer(window);
			
			WindowCloseEvent event;
			data.eventCallback(event);
		});
	}
}