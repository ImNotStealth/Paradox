#include "pxpch.h"
#include "OpenGLSwapChain.h"

#include "Paradox/Core/Application.h"

#include <glad/glad.h>

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