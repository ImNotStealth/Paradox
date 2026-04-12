#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/Window.h"
#include "Paradox/Events/ApplicationEvents.h"

namespace Paradox {

	class PARADOX_API Application
	{
	public:
		Application(const WindowCreateProperties& windowProps);
		virtual ~Application() {}

		void OnEvent(Event& event);

		void Stop();

		virtual void Run() {}
		virtual bool OnResize(WindowResizeEvent& e) { return false; }

		inline Window& GetWindow() { return *m_Window; }
		inline bool IsRunning() { return m_Running; }

		static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResized(WindowResizeEvent& e) { return OnResize(e); }

	private:
		Unique<Window> m_Window;
		bool m_Running = true;
		static Application* s_Instance;
	};

	Application* CreateApplication();
}