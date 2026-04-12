#include "pxpch.h"
#include "Application.h"

#include "Paradox/Core/Log.h"
#include "Paradox/Core/Version.h"

namespace Paradox {

	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowCreateProperties& windowProps)
	{
		PX_CORE_ASSERT(!s_Instance, "Application instance already exists.");
		PX_CORE_INFO("Paradox Engine {0} ({1}, {2})", PX_VERSION, PX_PLATFORM_NAME, PX_BUILD_CONFIG_NAME);
		PX_CORE_INFO("Starting Application");
		s_Instance = this;

		GraphicsContext::SetGraphicsAPI(GraphicsAPIType::Vulkan);

		m_Window = Window::Create(windowProps);
		m_Window->Init();
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
