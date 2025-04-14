#pragma once

#include "bcpch.h"

#include "Event.h"


namespace Bacon {

	class BACON_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		inline int GetMods() const { return m_mods; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keycode, int mods = None)
			: m_KeyCode(keycode), m_mods(mods){}

		int m_KeyCode;
		int m_mods;
	};

	class BACON_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount, int mods = None)
			: KeyEvent(keycode, mods), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(keyPressed)
	private:
		int m_RepeatCount;
	};

	class BACON_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode, int mods = None)
			: KeyEvent(keycode, mods) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class BACON_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode, int mods = None)
			: KeyEvent(keycode, mods) {
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}