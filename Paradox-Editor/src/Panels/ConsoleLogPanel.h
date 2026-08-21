#pragma once

#include "Panels/Panel.h"

#include <Paradox/Events/ApplicationEvents.h>
#include <imgui.h>

namespace Paradox
{
	class ConsoleLogPanel : public Panel
	{
	public:
		ConsoleLogPanel()
			: Panel("Console")
		{
			m_LogEvents.reserve(1000);
		}

		void OnEvent(Event& event) override;
		void OnImGuiRender(bool* opened) override;

	private:
		bool OnConsoleLog(ConsoleLogEvent& e);
		void DrawLogEntry(uint32_t index);
		void UpdateFilteredIndices();
		ImVec4 GetLogLevelColor(Log::Level level);

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