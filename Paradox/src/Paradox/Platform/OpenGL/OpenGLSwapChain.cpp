#include "pxpch.h"
#include "OpenGLSwapChain.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"

#ifdef PX_INCLUDE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace Paradox
{
	void OpenGLSwapChain::Create(uint32_t width, uint32_t height, bool vsync)
	{
		m_Width = width;
		m_Height = height;
		m_VSync = vsync;

#ifdef PX_INCLUDE_GLFW
		glfwSwapInterval(vsync ? 1 : 0);
#endif

		glViewport(0, 0, width, height);
		PX_CORE_TRACE("Created SwapChain: {0}x{1} (VSync: {2})", m_Width, m_Height, m_VSync);
	}

	void OpenGLSwapChain::OnResize(uint32_t width, uint32_t height)
	{
		Create(width, height, m_VSync);
	}

	void OpenGLSwapChain::RequestResize()
	{
		m_ResizeRequested = true;
	}

	void OpenGLSwapChain::SetVSync(bool enabled)
	{
		m_VSync = enabled;
		RequestResize();
	}

	void OpenGLSwapChain::Begin()
	{
		// Clearing to this to show that the swapchain has not received any render commands (a swapchainTarget framebuffer would override this clear color).
#ifndef PX_PLATFORM_PSVITA
		// Apparently clearing here messes up rendering to offscreen framebuffers on Vita (no clue if this is a bug or not but not clearing fixes everything so :shrug:)
		glClearColor(0.7f, 0.f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
#endif
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