#include "pxpch.h"
#include "GLFWWindow.h"

#include "Paradox/Events/ApplicationEvents.h"
#include "Paradox/Events/InputEvents.h"
#include "Paradox/Renderer/GraphicsContext.h"

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
		m_WindowData.fullscreen = props.fullscreen;
		m_WindowData.maximized = props.maximized;
		m_WindowData.vsync = props.vsync;
	}

	GLFWWindow::~GLFWWindow()
	{
		m_SwapChain.reset();
		m_GraphicsContext.reset();

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

		GraphicsAPIType apiType = GraphicsContext::GetGraphicsAPI();
		glfwWindowHint(GLFW_CLIENT_API, apiType == GraphicsAPIType::Vulkan ? GLFW_NO_API : GLFW_OPENGL_API);

		if (m_WindowData.fullscreen)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			m_Window = glfwCreateWindow(mode->width, mode->height, m_WindowData.title.c_str(), monitor, nullptr);
		}
		else
			m_Window = glfwCreateWindow(m_WindowData.width, m_WindowData.height, m_WindowData.title.c_str(), nullptr, nullptr);
		
		if (m_WindowData.maximized)
			glfwMaximizeWindow(m_Window);

		s_GLFWWindowCount++;

		glfwSetWindowUserPointer(m_Window, &m_WindowData);

		AssignCallbacks();

		m_GraphicsContext = GraphicsContext::Create();
		m_GraphicsContext->Init();

		m_SwapChain = SwapChain::Create();
		m_SwapChain->Init(this);
		m_SwapChain->Create(m_WindowData.width, m_WindowData.height, m_WindowData.vsync);

		PX_CORE_INFO("Created Window {0} {1}x{2}", m_WindowData.title, m_WindowData.width, m_WindowData.height);
	}

	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();

		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::OpenGL)
			glfwSwapBuffers(m_Window);
	}

	void GLFWWindow::WaitEvents()
	{
		glfwWaitEvents();
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		m_SwapChain->SetVSync(enabled);
		m_WindowData.vsync = enabled;
	}

	void GLFWWindow::Maximize()
	{
		glfwMaximizeWindow(m_Window);
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

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			GLFWWindowData& data = *(GLFWWindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressEvent event(key, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressEvent event(key, 1);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleaseEvent event(key);
				data.eventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			GLFWWindowData& data = *(GLFWWindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.eventCallback(event);
		});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
		{
			GLFWWindowData& data = *(GLFWWindowData*)glfwGetWindowUserPointer(window);

			WindowIconifyEvent event(iconified == GLFW_TRUE);
			data.eventCallback(event);
		});
	}
}