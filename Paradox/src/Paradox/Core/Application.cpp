#include "pxpch.h"
#include "Application.h"

namespace Paradox {

	Application::Application(const WindowCreateProperties& windowProps)
	{
		PX_CORE_INFO("Starting Application");

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
