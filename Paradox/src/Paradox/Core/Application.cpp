#include "pxpch.h"
#include "Application.h"

#include "Paradox/Core/Log.h"

namespace Paradox {

	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowCreateProperties& windowProps)
	{
		PX_CORE_ASSERT(!s_Instance, "Application instance already exists.");
		PX_CORE_INFO("Starting Application");
		s_Instance = this;

		m_Window = Window::Create(windowProps);
		m_Window->SetEventCallback(PX_BIND_EVENT_FN(Application::OnEvent));
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(PX_BIND_EVENT_FN(Application::OnWindowResized));
		dispatcher.Dispatch<WindowCloseEvent>(PX_BIND_EVENT_FN(Application::OnWindowClosed));
	}

	void Application::Stop()
	{
		PX_CORE_INFO("Stopping Application");
		m_Running = false;
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		Stop();
		return true;
	}
}
