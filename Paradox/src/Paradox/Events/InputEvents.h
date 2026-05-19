#pragma once

#include "Paradox/Events/Event.h"
#include "Paradox/Core/Input.h"

#include <sstream>

namespace Paradox
{
	class KeyEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeyCode keycode)
			: m_KeyCode(keycode) {}

	protected:
		KeyCode m_KeyCode;
	};

	class KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(KeyCode keycode, uint16_t repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) { }

		inline uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		uint16_t m_RepeatCount;
	};

	class KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(KeyCode keycode)
			: KeyEvent(keycode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleaseEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTypedEvent)
	};
}