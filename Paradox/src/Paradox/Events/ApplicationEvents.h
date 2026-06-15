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

	class ConsoleLogEvent : public Event
	{
	public:
		ConsoleLogEvent(Log::Domain domain, Log::Level level, const std::string& message)
			: m_Domain(domain), m_Level(level), m_Message(message) {}

		inline Log::Domain GetDomain() const { return m_Domain; }
		inline Log::Level GetLevel() const { return m_Level; }
		inline const std::string& GetLogMessage() const { return m_Message; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ConsoleLogEvent: [" << m_Level << "] " << m_Domain << ": " << m_Message;
			return ss.str();
		}

		EVENT_CLASS_TYPE(ConsoleLog)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		Log::Domain m_Domain;
		Log::Level m_Level;
		std::string m_Message;
	};
}