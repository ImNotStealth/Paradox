#include "pxpch.h"
#include "ParseDumpPanel.h"

#include <Paradox/Core/FileDialog.h>
#include <misc/cpp/imgui_stdlib.h>
#include <regex>

namespace Paradox
{
	void ParseDumpPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(600.f, 400.f), ImVec2(2000.f, 800.f));
		if (ImGui::BeginPopupModal(m_Name.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("Executable Path:");
			std::string path = m_ExecutablePath.string();
			if (ImGui::InputText("##Path", &path))
				m_ExecutablePath = path;
			ImGui::SameLine();
			if (ImGui::Button("Browse"))
			{
				std::filesystem::path selectedPath = FileDialog::SelectFile("Select PS Vita Executable", "PS Vita Executable (no extension)|*");
				if (!selectedPath.empty())
					m_ExecutablePath = selectedPath.string();
			}

			ImGui::Dummy({ 0.f, 10.f });

			ImVec2 contentAvail = ImGui::GetContentRegionAvail();
			contentAvail.y -= 36.f;
			if (ImGui::BeginTable("TwoColumnTable", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY, contentAvail))
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Raw dump");
				static std::string previousDump;
				ImGui::InputTextMultiline("##Editable", &m_RawDump, ImGui::GetContentRegionAvail());
				if (previousDump != m_RawDump)
				{
					previousDump = m_RawDump;
					PX_WARN("Parse dump");
					ParseDump();
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Parsed dump");
				ImGui::InputTextMultiline("##ReadOnly", &m_ParsedDump, ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);

				ImGui::EndTable();
			}

			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 buttonSize = ImVec2(80.f, 0.f);
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPos(ImVec2(windowSize.x - buttonSize.x - spacing - 6.f, windowSize.y - 36.f));

			if (ImGui::Button("Close", buttonSize))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}

	void ParseDumpPanel::ParseDump()
	{
		m_ParsedDump.clear();
		if (m_RawDump.empty())
			return;

		static const std::regex hexPattern(R"(0x[0-9a-fA-F]+)");

		auto it  = std::sregex_iterator(m_RawDump.begin(), m_RawDump.end(), hexPattern);
		auto end = std::sregex_iterator();

		size_t lastPos = 0;
		for (; it != end; ++it)
		{
			const std::smatch& match = *it;

			// Copy the text before this match unchanged
			m_ParsedDump.append(m_RawDump, lastPos, match.position() - lastPos);

			// Replace the hex code with your formatted version
			m_ParsedDump.append(ParseAddress(match.str()));

			lastPos = match.position() + match.length();
		}

		// Copy whatever's left after the last match
		m_ParsedDump.append(m_RawDump, lastPos, std::string::npos);
	}

	std::string ParseDumpPanel::ParseAddress(const std::string& address)
	{
		std::string cmd = "addr2line -e " + m_ExecutablePath.string() + " -f -C " + address;
		FILE* console = popen(cmd.c_str(), "r");
		if (!console)
		{
			PX_ERROR("Failed to run command.");
			return address;
		}

		std::array<char, 1024> buffer;
		std::string result;
		while (fgets(buffer.data(), buffer.size(), console) != nullptr)
			result += buffer.data();

		int status = pclose(console);
		if (status != 0)
			return "Unknown";

		// The dialog leaves a trailing \n character so this removes it
		while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
			result.pop_back();

		if (result.find("??") != std::string::npos)
			return address;

		return result;
	}
}
