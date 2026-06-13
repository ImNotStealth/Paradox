#pragma once

#include "Paradox/Events/Event.h"
#include <sstream>

namespace Paradox
{
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		uint32_t m_Width, m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowIconifyEvent : public Event
	{
	public:
		WindowIconifyEvent(bool minimized)
			: m_Minimized(minimized) {}

		inline bool IsMinimized() const { return m_Minimized; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowIconifyEvent: " << m_Minimized;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowIconify)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		bool m_Minimized;
	};
}