#pragma once

#include "Paradox/Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Paradox
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowCreateProperties& props);
		~GLFWWindow() override;

		void Init() override;
		const std::string& GetWindowTitle() const override { return m_WindowData.title; }
		uint32_t GetWidth() const override { return m_WindowData.width; }
		uint32_t GetHeight() const override { return m_WindowData.height; }
		void* GetHandle() const override { return m_Window; }
		Shared<GraphicsContext> GetGraphicsContext() override { return m_GraphicsContext; }
		Shared<SwapChain> GetSwapChain() override { return m_SwapChain; }

		//TODO: Temporary
		void WaitEvents() override;
		void PollEvents() override;

		void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.eventCallback = callback; }

	private:
		void AssignCallbacks();

	private:
		struct GLFWWindowData
		{
			std::string title;
			uint32_t width, height;
			EventCallbackFn eventCallback;
		};

		GLFWWindowData m_WindowData;
		GLFWwindow* m_Window = nullptr;

		//TODO: Check if a window should own the graphics context
		Shared<GraphicsContext> m_GraphicsContext;
		Shared<SwapChain> m_SwapChain;
	};
}