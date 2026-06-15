#include "pxpch.h"
#include "ConsoleLogPanel.h"

#include <Paradox/Events/ApplicationEvents.h>
#include <imgui.h>

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

		if (ImGui::Button("Clear"))
			m_LogEvents.clear();

		ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("##ConsoleLogPanel", 3, flags, ImGui::GetContentRegionAvail()))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("Domain", ImGuiTableColumnFlags_WidthFixed, 80.f);
			ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 80.f);
			ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 15.f, 5.f });

			ImGuiListClipper clipper;
			clipper.Begin(m_LogEvents.size());
			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					ImGui::TableNextRow();
					const LogEntry& e = m_LogEvents[row];

					ImGui::TableSetColumnIndex(0);
					ImGui::Selectable(("##LogEntry" + std::to_string(row)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Copy"))
							ImGui::SetClipboardText(e.message.c_str());

						if (ImGui::MenuItem("Copy All"))
						{
							std::string msg = "[" + Log::Get().LevelToString(e.level) + "] [" + Log::Get().DomainToString(e.domain) + "] " + e.message;
							ImGui::SetClipboardText(msg.c_str());
						}

						ImGui::EndPopup();
					}
					
					ImGui::SameLine();
					ImGui::TextUnformatted(Log::Get().DomainToString(e.domain).c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(Log::Get().LevelToString(e.level).c_str());

					ImGui::TableSetColumnIndex(2);
					ImGui::TextUnformatted(e.message.c_str());
				}
			}
			ImGui::PopStyleVar();
			ImGui::EndTable();
		}
		ImGui::End();
	}

	bool ConsoleLogPanel::OnConsoleLog(ConsoleLogEvent& e)
	{
		m_LogEvents.push_back({ e.GetDomain(), e.GetLevel(), e.GetLogMessage() });
		return false;
	}
}