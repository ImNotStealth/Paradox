#include "PackageProjectPanel.h"

#include "Project/Project.h"

#include <Paradox/Core/FileSystem.h>

namespace Paradox
{
	void PackageProjectPanel::OnImGuiRender(bool* opened, float deltaTime)
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

            if (ImGui::Button("Package"))
                Package();

            ImGui::EndPopup();
        }
	}

    void PackageProjectPanel::Package()
    {
        if (m_IsCompiling)
        {
            PX_ERROR("Failed to build Project: Already compiling.");
            return;
        }

        PX_WARN("Packaging Project: {0}", Project::GetActive()->GetProperties().name);
        
#ifdef PX_PLATFORM_WINDOWS // Didnt finish testing on Windows to test on Linux yet
        std::filesystem::path sourcePath = std::filesystem::path("C:\\Users\\Stealth\\Desktop\\Data\\Code\\Misc\\Paradox");
        std::filesystem::path exportPath = std::filesystem::current_path() / "Build";
        std::string cmd = "cmake -S " + sourcePath.string() + " -B " + exportPath.string() + " -G \"Visual Studio 18 2026\" -DPX_INCLUDE_EDITOR=OFF -DPX_INCLUDE_TRACY=OFF -DPX_INCLUDE_IMGUI=OFF && cmake --build " + exportPath.string() + " --config Release --target Runtime";
        PX_WARN("Compiling with Command: {0}", cmd);

        m_CompileThread = std::thread([exportPath, cmd, this]()
        {
            FILE* pipe = _popen((cmd + " 2>&1").c_str(), "r");
            if (!pipe)
                return;

            m_IsCompiling = true;

            char buffer[512];
            while (fgets(buffer, sizeof(buffer), pipe))
            {
                std::string line(buffer);
                while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
                    line.pop_back();
                if (line.find("error") != std::string::npos)
                    PX_ERROR(line);
                else
                    PX_INFO(line);
            }

            const int exitCode = _pclose(pipe);
            const std::string exitMsg = "Finished packaging with exit code: {0}";
            if (exitCode == 0)
            {
                PX_INFO(exitMsg, exitCode);
                FileSystem::ShowFolder(exportPath / "bin");
            }
            else
                PX_ERROR(exitMsg, exitCode);

            m_IsCompiling = false;
        });
        m_CompileThread.detach();
#endif
    }
}