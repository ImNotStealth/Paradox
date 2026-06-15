#pragma once

#include <Paradox/Core/Base.h>
#include <Paradox/Events/ApplicationEvents.h>

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

	private:
		struct LogEntry
		{
			Log::Domain domain;
			Log::Level level;
			std::string message;
		};
		std::vector<LogEntry> m_LogEvents = std::vector<LogEntry>();
	};
}