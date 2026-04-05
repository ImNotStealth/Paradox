#pragma once

#include "Paradox/Core/Window.h"
#include "Paradox/Renderer/GraphicsContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Paradox
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowCreateProperties& props);
		~GLFWWindow() override;

		const std::string& GetWindowTitle() const override { return m_WindowData.Title; }
		uint32_t GetWidth() const override { return m_WindowData.Width; }
		uint32_t GetHeight() const override { return m_WindowData.Height; }
		void* GetHandle() const override { return m_Window; }

		//TODO: Temporary
		void WaitEvents() override;
		void CreateSurface(void* instance, void* surface) override;
		void PollEvents() override;

		void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.EventCallback = callback; }

	private:
		void AssignCallbacks();

	private:
		struct GLFWWindowData
		{
			std::string Title;
			uint32_t Width, Height;

			EventCallbackFn EventCallback;
		};

		GLFWWindowData m_WindowData;
		GLFWwindow* m_Window = nullptr;

		//TODO: Check if a window should own the graphics context
		Unique<GraphicsContext> m_GraphicsContext;
	};
}