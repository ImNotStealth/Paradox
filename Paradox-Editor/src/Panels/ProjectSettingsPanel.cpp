#include "ProjectSettingsPanel.h"

#include "EditorApp.h"
#include "Project/Project.h"
#include "Utils/EditorIcons.h"

#include <Paradox/ImGui/ImGuiUtils.h>
#include <Paradox/Core/FileSystem.h>
#include <misc/cpp/imgui_stdlib.h>


namespace Paradox
{
    ProjectSettingsPanel::ProjectSettingsPanel()
        : Panel("Project Settings"), m_AppRef((EditorApp&)Application::Get())
    {
        m_SettingsCategories.emplace("Description", PX_BIND_FN(DrawDescriptionSettings));
        m_SettingsCategories.emplace("Runtime", PX_BIND_FN(DrawRuntimeSettings));
        m_SettingsCategories.emplace("Package", PX_BIND_FN(DrawPackageSettings));
        m_SettingsCategories.emplace("Windows", PX_BIND_FN(DrawWindowsSettings));
        m_SettingsCategories.emplace("Linux", PX_BIND_FN(DrawLinuxSettings));
        m_SettingsCategories.emplace("PlayStation Vita", PX_BIND_FN(DrawVitaSettings));
        m_SelectedCategory = "Description";

        m_VitaSplashDefault = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_VITA_SPLASH));
        m_VitaBubbleDefault = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_VITA_BUBBLE));
        m_VitaBackgroundDefault = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_VITA_BACKGROUND));
        m_VitaLaunchDefault = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_VITA_LAUNCH));
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
            props.startScene = UUID(sceneUuid);
        ImGui::SameLine();
        if (ImGui::Button("Use Current Scene"))
            props.startScene = !m_AppRef.GetScene() ? UUID::EMPTY : m_AppRef.GetScene()->GetAssetHandle();
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
            props.startScene = UUID::EMPTY;
    }

    void ProjectSettingsPanel::DrawPackageSettings()
    {
        PlatformPackageConfigProperties& props = Project::GetActive()->GetPlatformPackageProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Build Config");
        ImGuiUtils::HelpMarker("Debug: Includes Debug Symbols and all debugging features\nRelease: Excludes Debug Symbols but keeps debugging features\nDist: Production Build (Enabling Full Rebuild is recommended)");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Full Rebuild");
        ImGuiUtils::HelpMarker("Clears the build cache if enabled when packaging the project, disable for Dist builds to prevent issues.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Build DLL");
        ImGuiUtils::HelpMarker("Should the Engine DLL be a separate file or included in the executable?");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include ImGui");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include Tracy");
        ImGuiUtils::HelpMarker("Will have no effect on Dist builds.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include SpdLog");
        ImGuiUtils::HelpMarker("Will have no effect on Dist builds.");

        ImGui::TableNextColumn();

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

    void ProjectSettingsPanel::DrawWindowsSettings()
    {
        WindowsPackageConfigProperties& props = Project::GetActive()->GetPlatformPackageProperties().windowsProperties;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Application Icon");
        ImGuiUtils::HelpMarker("The icon must be a .ico file and be in the project's Assets folder.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Default Graphics API");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include Vulkan");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include OpenGL");

        ImGui::TableNextColumn();

        std::string appIconPath = props.iconFilePath.string();
        if (ImGui::InputText("##AppIconPath", &appIconPath))
            props.iconFilePath = appIconPath;
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            const std::filesystem::path path = FileSystem::SelectFile("Select Application Icon", "Icon|*.ico");
            if (!path.empty())
            {
                const std::filesystem::path relPath = std::filesystem::relative(path, Project::GetActive()->GetProperties().path);
                if (relPath.empty())
                    PX_ERROR("The icon must be in the Project's Assets folder.");
                else
                    props.iconFilePath = relPath.generic_string();
            }
        }

        const char* graphicApis[] = { "Vulkan", "OpenGL" };
        int graphicsApiIndex = (int)props.defaultGraphicsAPI - 1; // -1 to remove "None"
        if (ImGui::Combo("##DefaultGraphicsAPI", &graphicsApiIndex, graphicApis, IM_ARRAYSIZE(graphicApis)))
            props.defaultGraphicsAPI = (GraphicsAPIType)(graphicsApiIndex + 1); // +1 to add "None"

        ImGui::Checkbox("##IncludeVulkan", &props.includeVulkan);
        ImGui::Checkbox("##IncludeOpenGL", &props.includeOpenGL);
    }

    void ProjectSettingsPanel::DrawLinuxSettings()
    {
        LinuxPackageConfigProperties& props = Project::GetActive()->GetPlatformPackageProperties().linuxProperties;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Default Graphics API");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include Vulkan");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Include OpenGL");

        ImGui::TableNextColumn();

        const char* graphicApis[] = { "Vulkan", "OpenGL" };
        int graphicsApiIndex = (int)props.defaultGraphicsAPI - 1; // -1 to remove "None"
        if (ImGui::Combo("##DefaultGraphicsAPI", &graphicsApiIndex, graphicApis, IM_ARRAYSIZE(graphicApis)))
            props.defaultGraphicsAPI = (GraphicsAPIType)(graphicsApiIndex + 1); // +1 to add "None"

        ImGui::Checkbox("##IncludeVulkan", &props.includeVulkan);
        ImGui::Checkbox("##IncludeOpenGL", &props.includeOpenGL);
    }

    void ProjectSettingsPanel::DrawVitaSettings()
    {
        VitaPackageConfigProperties& props = Project::GetActive()->GetPlatformPackageProperties().vitaProperties;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("TitleID");
        ImGuiUtils::HelpMarker("This should be unique to your app.\nShould follow a format of 4 chars for the company and 5 for a build number.\nExample: PRDX00001");
        ImGui::TableSetColumnIndex(1);
        ImGui::InputText("##TitleID", &props.titleID);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Splash");
        ImGuiUtils::HelpMarker("Required dimensions: 960x544\nFormat: PNG, RGB");
        ImGui::TableSetColumnIndex(1);
        ImGuiUtils::TextureSlot(props.splashTexture, m_VitaSplashDefault, 160.f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Bubble Icon");
        ImGuiUtils::HelpMarker("Required dimensions: 128x128\nFormat: PNG, 256 Colors");
        ImGui::TableSetColumnIndex(1);
        ImGuiUtils::TextureSlot(props.bubbleIconTexture, m_VitaBubbleDefault, 160.f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("LiveArea Background");
        ImGuiUtils::HelpMarker("Required dimensions: 840x500");
        ImGui::TableSetColumnIndex(1);
        ImGuiUtils::TextureSlot(props.backgroundTexture, m_VitaBackgroundDefault, 160.f);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Launch button");
        ImGuiUtils::HelpMarker("Required dimensions: 280x158");
        ImGui::TableSetColumnIndex(1);
        ImGuiUtils::TextureSlot(props.launchTexture, m_VitaLaunchDefault, 160.f);

        // Store references of all the textures
        AssetManager* assetManager = AssetManager::Get();
        m_VitaSplashRef = assetManager->GetAsset<Texture2D>(props.splashTexture);
        m_VitaBubbleRef = assetManager->GetAsset<Texture2D>(props.bubbleIconTexture);
        m_VitaBackgroundRef = assetManager->GetAsset<Texture2D>(props.backgroundTexture);
        m_VitaLaunchRef = assetManager->GetAsset<Texture2D>(props.launchTexture);
    }
}
