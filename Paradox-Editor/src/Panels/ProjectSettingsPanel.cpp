#include "ProjectSettingsPanel.h"

#include "Project/Project.h"

#include <Paradox/ImGui/ImGuiUtils.h>
#include <Paradox/Core/FileSystem.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
    ProjectSettingsPanel::ProjectSettingsPanel()
        : Panel("Project Settings")
    {
        m_SettingsCategories.emplace("Description", PX_BIND_FN(ProjectSettingsPanel::DrawDescriptionSettings));
        m_SettingsCategories.emplace("Runtime", PX_BIND_FN(ProjectSettingsPanel::DrawRuntimeSettings));
        m_SettingsCategories.emplace("Windows", PX_BIND_FN(ProjectSettingsPanel::DrawWindowsSettings));
        m_SettingsCategories.emplace("PlayStation Vita", PX_BIND_FN(ProjectSettingsPanel::DrawRuntimeSettings));
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
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Copyright");

        ImGui::TableNextColumn();

        ImGui::InputText("##ProjectVersion", &props.version);
        ImGui::InputText("##Author", &props.author);
        ImGui::InputText("##Copyright", &props.copyright);
    }

    void ProjectSettingsPanel::DrawRuntimeSettings()
    {
        ProjectProperties& props = Project::GetActive()->GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Start Scene");
        ImGuiUtils::HelpMarker("Right click a Scene in the Asset Browser and click 'Copy UUID' to obtain its ID.");

        ImGui::TableNextColumn();
        
        std::string sceneUuid = props.startScene.ToString();
        if (ImGui::InputText("##SceneUUID", &sceneUuid))
            Project::GetActive()->GetProperties().startScene = UUID(sceneUuid);
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            Project::GetActive()->GetProperties().startScene = UUID::EMPTY;
    }

    void ProjectSettingsPanel::DrawWindowsSettings()
    {
        WindowsPackageConfigProperties& props = Project::GetActive()->GetPlatformPackageProperties().windowsProperties;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Application Icon");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Build Config");
        ImGuiUtils::HelpMarker("Debug: Includes Debug Symbols and all debugging features\nRelease: Excludes Debug Symbols but keeps debugging features\nDist: Production Build (Enabling Full Rebuild is recommended)");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Full Rebuild");
        ImGuiUtils::HelpMarker("Clears the build cache if enabled when packaging the project, disable for Dist builds to prevent issues.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Build DLL");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include ImGui");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include Tracy");
        ImGuiUtils::HelpMarker("Will have no effect on Dist builds.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include SpdLog");
        ImGuiUtils::HelpMarker("Will have no effect on Dist builds.");

        ImGui::TableNextColumn();

        std::string appIconPath = props.iconFilePath.string();
        if (ImGui::InputText("##AppIconPath", &appIconPath))
            props.iconFilePath = appIconPath;
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            std::filesystem::path path = FileSystem::SelectFile("Select Application Icon", "Icon|*.ico");
            if (!path.empty())
                props.iconFilePath = std::filesystem::relative(path, Project::GetActive()->GetProperties().path).generic_string();
        }

        const char* buildConfigs[] = { "Debug", "Release", "Dist" };
        int buildConfigIndex = 0;
        for (int i = 0; i < IM_ARRAYSIZE(buildConfigs); i++)
        {
            if (props.buildConfig == buildConfigs[i])
            {
                buildConfigIndex = i;
                break;
            }
        }
        if (ImGui::Combo("##BuildConfig", &buildConfigIndex, buildConfigs, IM_ARRAYSIZE(buildConfigs)))
            props.buildConfig = buildConfigs[buildConfigIndex];
        ImGui::Checkbox("##FullRebuild", &props.fullRebuild);
        ImGui::Checkbox("##BuildDLL", &props.buildDll);
        ImGui::Checkbox("##IncludeImGui", &props.includeImGui);
        ImGui::Checkbox("##IncludeTracy", &props.includeTracy);
        ImGui::Checkbox("##IncludeSpdLog", &props.includeSpdLog);
    }
}
