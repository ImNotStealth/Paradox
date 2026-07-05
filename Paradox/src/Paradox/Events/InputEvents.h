#pragma once

#include "Paradox/Events/Event.h"
#include "Paradox/Core/Input.h"

#include <sstream>

namespace Paradox
{
	class KeyCodeEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyCodeEvent(KeyCode keycode)
			: m_KeyCode(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << GetName() << ": " << m_KeyCode;
			return ss.str();
		}

	protected:
		KeyCode m_KeyCode;
	};

	class KeyPressEvent : public KeyCodeEvent
	{
	public:
		KeyPressEvent(KeyCode keycode, uint16_t repeatCount)
			: KeyCodeEvent(keycode), m_RepeatCount(repeatCount) { }

		inline uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	private:
		uint16_t m_RepeatCount;
	};

	class KeyReleaseEvent : public KeyCodeEvent
	{
	public:
		KeyReleaseEvent(KeyCode keycode)
			: KeyCodeEvent(keycode) { }

		EVENT_CLASS_TYPE(KeyReleased)
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};

	class KeyTypedEvent : public KeyCodeEvent
	{
	public:
		KeyTypedEvent(KeyCode keycode)
			: KeyCodeEvent(keycode) {}

		EVENT_CLASS_TYPE(KeyTyped)
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};

	class MousePressEvent : public KeyCodeEvent
	{
	public:
		MousePressEvent(KeyCode keycode)
			: KeyCodeEvent(keycode) {}

		EVENT_CLASS_TYPE(MousePressed)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};

	class MouseReleaseEvent : public KeyCodeEvent
	{
	public:
		MouseReleaseEvent(KeyCode keycode)
			: KeyCodeEvent(keycode) {}

		EVENT_CLASS_TYPE(MouseReleased)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};
}