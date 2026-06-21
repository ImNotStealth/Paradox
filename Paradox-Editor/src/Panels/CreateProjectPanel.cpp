#include "pxpch.h"
#include "CreateProjectPanel.h"

#include "Project/Project.h"

#include <Paradox/Core/FileDialog.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
	void CreateProjectPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(400.f, 250.f), ImVec2(1000.f, 250.f));
		if (ImGui::BeginPopupModal(m_Name.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("Project Name:");
			ImGui::InputText("##Name", &m_ProjectName);
			
			ImGui::Dummy({ 0.f, 10.f });
			
			ImGui::Text("Project Path:");
			std::string path = m_ProjectPath.string();
			if (ImGui::InputText("##Path", &path))
				m_ProjectPath = path;
			ImGui::SameLine();
			if (ImGui::Button("Browse"))
			{
				std::filesystem::path selectedPath = FileDialog::SelectFolder("Select Project Path");
				if (!selectedPath.empty())
					m_ProjectPath = selectedPath.string();
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1.f));
			ImGui::Text("Project will be created at:");

			std::string separator(1, std::filesystem::path::preferred_separator);
			ImGui::Text("%s%s%s%s", m_ProjectPath.string().c_str(), separator.c_str(), m_ProjectName.c_str(), separator.c_str());
			ImGui::PopStyleColor();

			if (!IsNameValid(m_ProjectName))
				m_ErrorMessage = "Project name is empty or contains invalid characters.";
			else if (m_ProjectPath.empty())
				m_ErrorMessage = "Project path cannot be empty.";
			else if (std::filesystem::exists(m_ProjectPath / m_ProjectName))
				m_ErrorMessage = "Directory already exists.";

			if (!m_ErrorMessage.empty())
			{
				ImGui::Dummy({ 0.f, 10.f });
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.4f, 0.4f, 1.f));
				ImGui::Text("%s", m_ErrorMessage.c_str());
				ImGui::PopStyleColor();
			}

			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 buttonSize = ImVec2(80.f, 0.f);
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPos(ImVec2(windowSize.x - (buttonSize.x * 2.f) - spacing - 12.f, windowSize.y - 36.f));

			bool disabled = !m_ErrorMessage.empty();
			if (disabled)
				ImGui::BeginDisabled();

			if (ImGui::Button("Create", buttonSize))
			{
				ProjectProperties properties;
				properties.name = m_ProjectName;
				properties.path = m_ProjectPath / m_ProjectName;
				properties.assetPath = properties.path / "Assets";

				Project project(properties);
				Project::SetActive(project);
				ImGui::CloseCurrentPopup();
			}

			if (disabled)
				ImGui::EndDisabled();

			ImGui::SameLine();
			if (ImGui::Button("Cancel", buttonSize))
				ImGui::CloseCurrentPopup();

			m_ErrorMessage = std::string();

			ImGui::EndPopup();
		}
	}

	bool CreateProjectPanel::IsNameValid(const std::string& name)
	{
		if (name.empty())
			return false;

		return name.find_first_of("\\/:*?\"<>|") == std::string::npos;
	}
}