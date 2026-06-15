#pragma once

#include "Paradox/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Paradox
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowCreateProperties& props);
		~GLFWWindow() override;

		void Init() override;
		void OnUpdate() override;

		const std::string& GetWindowTitle() const override { return m_WindowData.title; }
		uint32_t GetWidth() const override { return m_WindowData.width; }
		uint32_t GetHeight() const override { return m_WindowData.height; }
		bool IsVSync() const override { return m_WindowData.vsync; }
		void* GetHandle() const override { return m_Window; }
		Shared<GraphicsContext> GetGraphicsContext() override { return m_GraphicsContext; }
		Shared<SwapChain> GetSwapChain() override { return m_SwapChain; }

		//TODO: Temporary
		void WaitEvents() override;

		void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.eventCallback = callback; }
		void SetVSync(bool enabled) override;

	private:
		void AssignCallbacks();

	private:
		struct GLFWWindowData
		{
			std::string title;
			uint32_t width, height;
			bool fullscreen, maximized, vsync;

			EventCallbackFn eventCallback;
		};

		GLFWWindowData m_WindowData;
		GLFWwindow* m_Window = nullptr;

		Shared<GraphicsContext> m_GraphicsContext;
		Shared<SwapChain> m_SwapChain;
	};
}