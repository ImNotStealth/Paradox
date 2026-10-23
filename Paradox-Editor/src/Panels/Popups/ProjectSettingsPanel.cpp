#include "ProjectSettingsPanel.h"

#include "Project/Project.h"

#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
    ProjectSettingsPanel::ProjectSettingsPanel()
        : Panel("Project Settings")
    {
        m_SettingsCategories.emplace("Description", PX_BIND_FN(ProjectSettingsPanel::DrawDescriptionSettings));
        m_SettingsCategories.emplace("Runtime", PX_BIND_FN(ProjectSettingsPanel::DrawRuntimeSettings));
        m_SelectedCategory = "Description";
    }

    void ProjectSettingsPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();
		if (ImGui::BeginPopupModal(m_Name.c_str(), opened))
		{
			if (!Project::GetActive())
			{
				ImGui::TextUnformatted("Open a Project to start.");
				ImGui::EndPopup();
				return;
			}

            // Left
            {
                ImGui::BeginChild("LeftPane", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
                for (const auto& [name, func] : m_SettingsCategories)
                {
                    if (ImGui::Selectable(name.c_str(), m_SelectedCategory == name, ImGuiSelectableFlags_SelectOnNav))
                        m_SelectedCategory = name;
                }
                ImGui::EndChild();
            }
            ImGui::SameLine();

            // Right
            {
                ImGui::BeginGroup();
                ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1], ImGui::GetFontSize() * 1.2f);
                ImGui::TextUnformatted(m_SelectedCategory.c_str());
                ImGui::PopFont();
                ImGui::Separator();

                if (ImGui::BeginTable("##Properties", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
                {
                    // Push object ID after we entered the table, so table is shared for all objects
                    //ImGui::PushID(5);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 2.0f); // Default twice larger

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    m_SettingsCategories[m_SelectedCategory]();

                    //ImGui::PopID();
                    ImGui::EndTable();
                }

                ImGui::EndChild();
                ImGui::EndGroup();
            }

			/**/

			ImGui::EndPopup();
		}
	}

    void ProjectSettingsPanel::OnClose()
    {
        Project::GetActive()->Serialize();
    }

    void ProjectSettingsPanel::DrawDescriptionSettings()
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Project Version");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Author(s)");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Window Name");

        ImGui::TableNextColumn();

        std::string projectVersion = "1.0";
        ImGui::InputText("##ProjectVersion", &projectVersion);

        std::string authors = "None";
        ImGui::InputText("##Authors", &authors);

        std::string windowName = "Niva";
        ImGui::InputText("##WindowName", &windowName);
    }

    void ProjectSettingsPanel::DrawRuntimeSettings()
    {
        ProjectProperties& props = Project::GetActive()->GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Start Scene");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Test2");

        ImGui::TableNextColumn();
        
        std::string sceneUuid = props.startScene.ToString();
        if (ImGui::InputText("##SceneUUID", &sceneUuid))
            Project::GetActive()->GetProperties().startScene = UUID(sceneUuid);
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            Project::GetActive()->GetProperties().startScene = UUID::EMPTY;

        bool b = true;
        ImGui::Checkbox("##Editor2", &b);
    }
}
