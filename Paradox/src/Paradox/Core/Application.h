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

		void Run();
		void Stop();

		void OnEvent(Event& event);

		virtual void OnUpdate(float deltaTime) {}

		inline Window& GetWindow() { return *m_Window; }
		inline bool IsRunning() { return m_Running; }

		static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowResized(WindowResizeEvent& e);
		bool OnWindowClosed(WindowCloseEvent& e);

	private:
		Unique<Window> m_Window;
		bool m_Running = true;
		float m_LastFrameTime = 0.f;

		static Application* s_Instance;
	};

	Application* CreateApplication();
}