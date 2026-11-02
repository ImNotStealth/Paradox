#include "PackageProjectPanel.h"

#include "Project/Project.h"
#include "Utils/EditorConfig.h"

#include <Paradox/Core/FileSystem.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cctype>
#include <algorithm>

namespace Paradox
{
	void PackageProjectPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
        PX_PROFILE_FUNCTION();
        ImGui::SetNextWindowSizeConstraints(ImVec2(600.f, 250.f), ImVec2(1000.f, 250.f));
        if (ImGui::BeginPopupModal(m_Name.c_str(), opened, ImGuiWindowFlags_NoSavedSettings))
        {
            if (!Project::GetActive())
            {
                ImGui::TextUnformatted("Open a Project to start.");
                ImGui::EndPopup();
                return;
            }

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::Text("Build Path");
            ImGui::PopFont();
            std::string path = m_TargetBuildPath.string();
            if (ImGui::InputText("##Path", &path))
                m_TargetBuildPath = path;
            ImGui::SameLine();
            if (ImGui::Button("..."))
            {
                std::filesystem::path selectedPath = FileSystem::SelectFolder("Select Build Path");
                if (!selectedPath.empty())
                    m_TargetBuildPath = selectedPath.string();
            }

            ImGui::TextDisabled("Project will be built at:");

            std::string separator(1, std::filesystem::path::preferred_separator);
            ImGui::TextDisabled("%s%s", m_TargetBuildPath.string().c_str(), separator.c_str());

            ImGui::Dummy({ 0.f, 10.f });
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.97f, 0.95f, 0.65f, 1.f));
            ImGui::TextUnformatted("Note: You cannot cancel a build once started.");
            ImGui::TextUnformatted("Note: Closing the Editor or this popup before a build is complete will leave it waiting until the build is finished.");
            ImGui::PopStyleColor();

            if (m_CaughtError)
            {
                ImGui::Dummy({ 0.f, 10.f });
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.4f, 0.4f, 1.f));
                ImGui::TextUnformatted("An error occured, check the Console for details.");
                ImGui::PopStyleColor();
            }

            ImGui::Checkbox("Include ImGui", &m_IncludeImGui);

            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 buttonSize = ImVec2(80.f, 0.f);
            float spacing = ImGui::GetStyle().ItemSpacing.x;
            ImGui::SetCursorPos(ImVec2(windowSize.x - buttonSize.x - spacing - 6.f, windowSize.y - 36.f));

            bool isBuilding = !m_CompileFuture.valid() ? false : m_CompileFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready;

            if (isBuilding)
                ImGui::BeginDisabled();

            if (ImGui::Button("Package", buttonSize))
                Package();

            if (isBuilding)
                ImGui::EndDisabled();

            ImGui::EndPopup();
        }
	}

    void PackageProjectPanel::Package()
    {
        if (m_CompileFuture.valid() && m_CompileFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
        {
            PX_ERROR("Failed to package Project: Already compiling.");
            return;
        }

        const std::string projectName = Project::GetActive()->GetProperties().name;
        PX_WARN("Packaging Project: {0}", projectName);

        if (!std::filesystem::exists(m_TargetBuildPath))
            std::filesystem::create_directories(m_TargetBuildPath);
        
        m_CompileFuture = std::async(std::launch::async, [projectName, this]
        {
            m_CaughtError = false;
            m_BinFilePath = std::filesystem::path();

            m_CaughtError |= Compile();

            std::filesystem::copy(m_BinFilePath.parent_path(), m_TargetBuildPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
            
            std::string extension = m_BinFilePath.has_extension() ? m_BinFilePath.extension().string() : "";
            std::filesystem::rename(m_TargetBuildPath / m_BinFilePath.filename(), m_TargetBuildPath / (projectName + extension));

            if (EditorConfig::GetProperties().openPackagedFolder)
                FileSystem::ShowFolder(m_TargetBuildPath);
        });
    }

    bool PackageProjectPanel::Compile()
    {
#ifdef PX_PLATFORM_WINDOWS // Didnt finish testing on Windows to test on Linux yet
        const std::filesystem::path sourcePath = EditorConfig::GetProperties().engineSourcePath;
        const std::filesystem::path compilePath = std::filesystem::current_path() / "Build";
        std::string cmd = "cmake -S " + sourcePath.string() + " -B " + compilePath.string() + " -G \"Visual Studio 18 2026\" -DPX_INCLUDE_EDITOR=OFF -DPX_INCLUDE_TRACY=OFF -DPX_INCLUDE_IMGUI=" + (m_IncludeImGui ? "ON" : "OFF") + " && cmake --build " + compilePath.string() + " --config Release --target Runtime";
        PX_WARN("Compiling with Command: {0}", cmd);

        FILE* pipe = _popen(("(" + cmd + ") 2>&1").c_str(), "r");
        if (!pipe)
            return true;

        const std::string binPathPrefix = "PX_BIN_PATH=";

        char buffer[512];
        while (fgets(buffer, sizeof(buffer), pipe))
        {
            std::string line(buffer);
            while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
                line.pop_back();

            const size_t pos = line.find(binPathPrefix);
            if (pos != std::string::npos)
                m_BinFilePath = line.substr(pos + binPathPrefix.size());

            std::string lineLower = line;
            std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(),
                [](unsigned char c) { return (unsigned char)std::tolower(c); });

            if (lineLower.find("error") != std::string::npos)
                PX_ERROR("{0}", line);
            else
                PX_INFO("{0}", line);
        }

        const int exitCode = _pclose(pipe);
        const std::string exitMsg = "Finished packaging with exit code: {0}";
        if (exitCode != 0)
        {
            PX_ERROR(exitMsg, exitCode);
            return true;
        }

        PX_INFO(exitMsg, exitCode);
        if (m_BinFilePath.empty())
        {
            PX_ERROR("Failed to package Project: Bin file path not found.");
            return true;
        }

        return false;
#endif
    }
}