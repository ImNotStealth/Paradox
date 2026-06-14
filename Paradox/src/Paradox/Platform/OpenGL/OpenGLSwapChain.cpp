#include "pxpch.h"
#include "OpenGLSwapChain.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"

namespace Paradox
{
	void OpenGLSwapChain::OnResize(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void OpenGLSwapChain::RequestResize()
	{
		m_ResizeRequested = true;
	}

	void OpenGLSwapChain::Begin()
	{
		// Clearing to this to show that the swapchain has not received any render commands (a swapchainTarget framebuffer would override this clear color).
		glClearColor(0.7f, 0.f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLSwapChain::End()
	{
		if (m_ResizeRequested)
		{
			m_ResizeRequested = false;
			const Window& window = Application::Get().GetWindow();
			OnResize(window.GetWidth(), window.GetHeight());
		}
	}
}