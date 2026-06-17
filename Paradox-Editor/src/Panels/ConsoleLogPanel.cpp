#include "pxpch.h"
#include "ConsoleLogPanel.h"

namespace Paradox
{
	void ConsoleLogPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<ConsoleLogEvent>(PX_BIND_EVENT_FN(ConsoleLogPanel::OnConsoleLog));
	}

	void ConsoleLogPanel::OnImGuiRender()
	{
		ImGui::Begin("Console");

		if (ImGui::Button("Clear Logs"))
		{
			m_LogEvents.clear();
			m_FilteredIndices.clear();
		}

		ImGui::SameLine();
		ImGui::Text("Total Logs: %ld", m_LogEvents.size());
		
		ImGui::SameLine();
		ImVec2 filterPos = ImGui::GetCursorScreenPos();

		float filterButtonWidth = ImGui::CalcTextSize("Settings").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float filterWidth = ImGui::GetContentRegionAvail().x - filterButtonWidth - ImGui::GetStyle().ItemSpacing.x;

		static std::string previousFilter;
		m_Filter.Draw("##ConsoleLogPanelFilter", filterWidth);
		if (previousFilter != m_Filter.InputBuf)
		{
			previousFilter = m_Filter.InputBuf;
			m_FilteredIndicesDirty = true;
		}
		if (!m_Filter.IsActive())
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddText({ filterPos.x + 5.f, filterPos.y + 4.f }, ImGui::GetColorU32(ImGuiCol_TextDisabled), "Search...");
		}

		ImGui::SameLine();
		if (ImGui::Button("Settings"))
			ImGui::OpenPopup("SettingsPopup");

		if (ImGui::BeginPopup("SettingsPopup"))
		{
			for (int i = 0; i < 5; i++)
			{
				uint8_t levelBit = m_LevelFilter & (1 << i);
				bool enabled = (m_LevelFilter & levelBit) != 0;
				if (ImGui::Checkbox(Log::Get().LevelToString((Log::Level)i).c_str(), &enabled))
				{
					m_LevelFilter ^= (1 << i);
					m_FilteredIndicesDirty = true;
				}
			}
			ImGui::Separator();
			ImGui::Checkbox("Auto Scroll", &m_AutoScroll);
			ImGui::EndPopup();
		}

		ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("##ConsoleLogPanel", 4, flags, ImGui::GetContentRegionAvail()))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("Domain", ImGuiTableColumnFlags_WidthFixed, 80.f);
			ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 80.f);
			ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 90.f);
			ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 15.f, 5.f });

			ImGuiListClipper clipper;
			clipper.Begin(m_FilteredIndices.size());
			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					DrawLogEntry(m_FilteredIndices[i]);
			}
			clipper.End();

			if (m_AutoScrollRequested)
			{
				ImGui::SetScrollHereY(1.0f);
				m_AutoScrollRequested = false;
			}

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}
		ImGui::End();

		if (m_FilteredIndicesDirty)
		{
			UpdateFilteredIndices();
			m_FilteredIndicesDirty = false;
		}
	}

	bool ConsoleLogPanel::OnConsoleLog(ConsoleLogEvent& e)
	{
		auto now = std::chrono::system_clock::now();
		time_t time = std::chrono::system_clock::to_time_t(now);
		tm* localTime = localtime(&time);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
		std::stringstream ss;
		ss << std::put_time(localTime, "%T")
			<< '.'
			<< std::setw(3)
			<< std::setfill('0')
			<< ms.count();

		m_LogEvents.push_back({ e.GetDomain(), e.GetLevel(), e.GetLogMessage(), ss.str() });
		m_AutoScrollRequested = m_AutoScroll;
		m_FilteredIndicesDirty = true;
		return false;
	}

	void ConsoleLogPanel::DrawLogEntry(uint32_t index)
	{
		PX_ASSERT(index < m_LogEvents.size(), "Index out of bounds");
		const LogEntry& entry = m_LogEvents[index];

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Selectable(("##LogEntry" + std::to_string(index)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy"))
				ImGui::SetClipboardText(entry.message.c_str());

			if (ImGui::MenuItem("Copy All"))
			{
				std::string msg = "[" + Log::Get().LevelToString(entry.level) + "] [" + Log::Get().DomainToString(entry.domain) + "] [" + entry.time + "] " + entry.message;
				ImGui::SetClipboardText(msg.c_str());
			}

			ImGui::EndPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, GetLogLevelColor(entry.level));
		ImGui::SameLine();
		ImGui::TextUnformatted(Log::Get().DomainToString(entry.domain).c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(Log::Get().LevelToString(entry.level).c_str());

		ImGui::TableSetColumnIndex(2);
		ImGui::TextUnformatted(entry.time.c_str());

		ImGui::TableSetColumnIndex(3);
		ImGui::TextUnformatted(entry.message.c_str());
		ImGui::PopStyleColor();
	}

	void ConsoleLogPanel::UpdateFilteredIndices()
	{
		m_FilteredIndices.clear();
		for (size_t i = 0; i < m_LogEvents.size(); i++)
		{
			const LogEntry& e = m_LogEvents[i];

			uint8_t levelBit = (1 << (uint8_t)e.level);
			if (!(m_LevelFilter & levelBit))
				continue;

			if (!m_Filter.PassFilter(e.message.c_str()))
				continue;

			m_FilteredIndices.push_back(i);
		}
	}

	ImVec4 ConsoleLogPanel::GetLogLevelColor(Log::Level level)
	{
		switch (level)
		{
		case Log::Level::Info:     return ImVec4(0.075f, 0.63f, 0.05f, 1.f);
		case Log::Level::Warn:     return ImVec4(0.97f, 0.95f, 0.65f, 1.f);
		case Log::Level::Error:    return ImVec4(0.9f, 0.3f, 0.34f, 1.f);
		case Log::Level::Critical: return ImVec4(0.8f, 0.1f, 0.1f, 1.f);
		
		case Log::Level::Trace:
		default:
			return ImVec4(1.f, 1.f, 1.f, 1.f);
		}
	}
}