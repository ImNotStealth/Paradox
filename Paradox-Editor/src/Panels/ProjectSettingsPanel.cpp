#include "ProjectSettingsPanel.h"

#include "Project/Project.h"

#include <Paradox/ImGui/ImGuiUtils.h>
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
        if (!ImGui::Begin(m_Name.c_str(), opened))
        {
            ImGui::End();
            return;
        }

        if (!Project::GetActive())
        {
            ImGui::TextUnformatted("Open a Project to start.");
            ImGui::End();
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

        ImGui::End();
	}

    void ProjectSettingsPanel::OnClose()
    {
        if (Project::GetActive())
            Project::GetActive()->Serialize();
    }

    void ProjectSettingsPanel::DrawDescriptionSettings()
    {
        ProjectProperties& props = Project::GetActive()->GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Project Version");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Author");

        ImGui::TableNextColumn();

        std::string version = props.version;
        if (ImGui::InputText("##ProjectVersion", &version))
            props.version = version;

        std::string author = props.author;
        if (ImGui::InputText("##Author", &author))
            props.author = author;
    }

    void ProjectSettingsPanel::DrawRuntimeSettings()
    {
        ProjectProperties& props = Project::GetActive()->GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Start Scene");
        ImGuiUtils::HelpMarker("Right click a Scene in the Asset Browser and click 'Copy UUID' to obtain its ID.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Window Name");

        ImGui::TableNextColumn();
        
        std::string sceneUuid = props.startScene.ToString();
        if (ImGui::InputText("##SceneUUID", &sceneUuid))
            Project::GetActive()->GetProperties().startScene = UUID(sceneUuid);
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            Project::GetActive()->GetProperties().startScene = UUID::EMPTY;

        std::string windowName = "Niva";
        ImGui::InputText("##WindowName", &windowName);
    }
}
