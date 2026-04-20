#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Events/Event.h"
#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Renderer/SwapChain.h"

namespace Paradox
{
	struct WindowCreateProperties
	{
		std::string title = "Paradox Engine";
		uint32_t width = 1600;
		uint32_t height = 800;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void Init() = 0;
		virtual const std::string& GetWindowTitle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void* GetHandle() const = 0;
		virtual Shared<GraphicsContext> GetGraphicsContext() = 0;
		virtual Shared<SwapChain> GetSwapChain() = 0;

		//TODO: Temporary
		virtual void WaitEvents() = 0;
		virtual void PollEvents() = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		static Unique<Window> Create(const WindowCreateProperties& props);
	};
}