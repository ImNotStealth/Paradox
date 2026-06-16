#pragma once

#include <Paradox/Core/Base.h>
#include <Paradox/Events/ApplicationEvents.h>

#include <imgui.h>

namespace Paradox
{
	class ConsoleLogPanel
	{
	public:
		ConsoleLogPanel()
		{
			m_LogEvents.reserve(1000);
		}

		void OnEvent(Event& event);
		void OnImGuiRender();

	private:
		bool OnConsoleLog(ConsoleLogEvent& e);
		void DrawLogEntry(uint32_t index);
		void UpdateFilteredIndices();

	private:
		struct LogEntry
		{
			Log::Domain domain;
			Log::Level level;
			std::string message;
			std::string time;
		};

		std::vector<LogEntry> m_LogEvents = std::vector<LogEntry>();
		std::vector<size_t> m_FilteredIndices = std::vector<size_t>();
		ImGuiTextFilter m_Filter;
		bool m_AutoScroll = true;
		bool m_AutoScrollRequested = false, m_FilteredIndicesDirty = false;
		uint8_t m_LevelFilter = 0xFF; // Every level enabled by default
	};
}