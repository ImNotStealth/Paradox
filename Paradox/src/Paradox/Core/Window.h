#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Events/Event.h"
#include "Paradox/Renderer/GraphicsContext.h"

namespace Paradox
{
	struct WindowCreateProperties
	{
		std::string Title = "Paradox Engine";
		uint32_t Width = 1600;
		uint32_t Height = 800;
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
		virtual Unique<GraphicsContext>& GetGraphicsContext() = 0;

		//TODO: Temporary
		virtual void WaitEvents() = 0;
		virtual void CreateSurface(void* instance, void* surface) = 0;
		virtual void PollEvents() = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		static Unique<Window> Create(const WindowCreateProperties& props);
	};
}