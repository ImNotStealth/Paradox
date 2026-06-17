#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/Window.h"
#include "Paradox/Core/CommandLineParser.h"
#include "Paradox/Events/ApplicationEvents.h"
#include "Paradox/Renderer/ImGuiRenderer.h"

namespace Paradox {

	class PARADOX_API Application
	{
	public:
		Application(const WindowCreateProperties& windowProps, const CommandLineParser& args);
		virtual ~Application() {}

		void Run();
		void Stop();

		virtual void OnEvent(Event& event) {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGuiRender(float deltaTime) {}

		inline Window& GetWindow() { return *m_Window; }
		inline bool IsRunning() { return m_Running; }
		inline bool IsMinimized() { return m_Minimized; }
		inline const CommandLineParser& GetCommandLineArgs() { return m_CommandLineArgs; }
		inline void* GetImGuiContext() { return m_ImGuiRenderer->GetContext(); }

		static Application& Get() { return *s_Instance; }

	private:
		void OnEventInternal(Event& event);
		bool OnWindowResized(WindowResizeEvent& e);
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowIconified(WindowIconifyEvent& e);

	private:
		Unique<Window> m_Window;
		CommandLineParser m_CommandLineArgs;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.f;
		Unique<ImGuiRenderer> m_ImGuiRenderer;

		static Application* s_Instance;
	};

	Application* CreateApplication(const CommandLineParser& args);
}