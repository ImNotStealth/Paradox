#include "EditorSettingsPanel.h"

#include "EditorApp.h"
#include "Utils/EditorConfig.h"

#include <Paradox/Core/Application.h>
#include <Paradox/Core/FileSystem.h>
#include <Paradox/ImGui/ImGuiUtils.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
    EditorSettingsPanel::EditorSettingsPanel()
        : Panel("Editor Settings"), m_AppRef((EditorApp&)Application::Get())
    {
        m_SettingsCategories.emplace("Renderer", PX_BIND_FN(EditorSettingsPanel::DrawRendererSettings));
        m_SettingsCategories.emplace("Viewport", PX_BIND_FN(EditorSettingsPanel::DrawViewportSettings));
        m_SettingsCategories.emplace("Package", PX_BIND_FN(EditorSettingsPanel::DrawPackageSettings));
        m_SelectedCategory = "Renderer";
    }

    void EditorSettingsPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();
        if (!ImGui::Begin(m_Name.c_str(), opened))
        {
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

    void EditorSettingsPanel::OnClose()
    {
        EditorConfig::Serialize();
    }

    void EditorSettingsPanel::DrawRendererSettings()
    {
        EditorConfigProperties& props = EditorConfig::GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Graphics API");
        ImGuiUtils::HelpMarker("Changing this setting will require an Editor restart to take effect.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("VSync");

        ImGui::TableNextColumn();

        const char* graphicApis[] = { "Vulkan", "OpenGL" };
        int graphicsApiIndex = (int)props.graphicsApi - 1; // -1 to remove "None"
        if (ImGui::Combo("##GraphicsAPI", &graphicsApiIndex, graphicApis, IM_ARRAYSIZE(graphicApis)))
            props.graphicsApi = (GraphicsAPIType)(graphicsApiIndex + 1); // +1 to add "None"

        if (ImGui::Checkbox("##VSync", &props.vsync))
            Application::Get().GetWindow().SetVSync(props.vsync);
    }

    void EditorSettingsPanel::DrawPackageSettings()
    {
        EditorConfigProperties& props = EditorConfig::GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Engine Source Path");
        ImGuiUtils::HelpMarker("You shoudn't need to change this setting unless you know what you are doing.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("CMake Generator");
        ImGuiUtils::HelpMarker("Which generator to use when packaging the project.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Open Packaged Folder");
        ImGuiUtils::HelpMarker("Should the packaged folder open when successfully packaging the project?");

        ImGui::TableNextColumn();
        
        std::string engineSourcePath = props.engineSourcePath.string();
        if (ImGui::InputText("##EngineSourcePath", &engineSourcePath))
            props.engineSourcePath = engineSourcePath;
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            std::filesystem::path selectedPath = FileSystem::SelectFolder("Select Engine Source Path");
            if (!selectedPath.empty())
                props.engineSourcePath = selectedPath.generic_string();
        }

        ImGui::InputText("##CMakeGenerator", &props.cmakeGenerator);
        ImGui::Checkbox("##OpenPackagedFolder", &props.openPackagedFolder);
    }

    void EditorSettingsPanel::DrawViewportSettings()
    {
        EditorConfigProperties& props = EditorConfig::GetProperties();

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Camera Speed");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Gizmo Snap");
        ImGuiUtils::HelpMarker("The Gizmo will snap to these values if LCTRL is held down.");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Draw Grid");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Grid Scale");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Grid Size");
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Grid Color");

        ImGui::TableNextColumn();

        if (ImGui::DragFloat("##CameraSpeed", &props.cameraSpeed, 0.01f))
            m_AppRef.GetCamera().SetSpeed(props.cameraSpeed);
        
        ImGui::SliderFloat("##SnapValue", &props.gizmoSnap, 0.1f, 10.f, "%.1f");
        ImGui::Checkbox("##DrawGrid", &props.drawGrid);
        ImGui::DragFloat("##GridScale", &props.gridScale);
        ImGui::DragFloat("##GridSize", &props.gridSize, 0.01f, 0.01f, 1.f);
        ImGui::ColorEdit4("##GridColor", glm::value_ptr(props.gridColor));
    }
}
