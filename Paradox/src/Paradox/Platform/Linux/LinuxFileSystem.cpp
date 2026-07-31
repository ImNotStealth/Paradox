#include "pxpch.h"
#include "Paradox/Core/FileSystem.h"

namespace Paradox
{
    std::filesystem::path FileSystem::SelectFile(const std::string& title, const std::string& filter)
    {
        std::string cmd = "zenity --file-selection --title='" + title + "\' --file-filter=\'" + filter + "\'";
        FILE* console = popen(cmd.c_str(), "r");
        if (!console)
        {
            PX_CORE_ERROR("Failed to run command.");
            return {};
        }

        std::array<char, 512> buffer;
        std::string result;
        while (fgets(buffer.data(), buffer.size(), console) != nullptr)
            result += buffer.data();

        int status = pclose(console);
        if (status != 0)
            return {}; // Dialog window was probably closed without selecting anything

        // The dialog leaves a trailing \n character so this removes it
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
            result.pop_back();

		return std::filesystem::path(result);
    }

    std::filesystem::path FileSystem::SelectFolder(const std::string& title)
    {
        std::string cmd = "zenity --file-selection --directory --title='" + title + "\'";
        FILE* console = popen(cmd.c_str(), "r");
        if (!console)
        {
            PX_CORE_ERROR("Failed to run command.");
            return {};
        }

        std::array<char, 512> buffer;
        std::string result;
        while (fgets(buffer.data(), buffer.size(), console) != nullptr)
            result += buffer.data();

        int status = pclose(console);
        if (status != 0)
            return {}; // Dialog window was probably closed without selecting anything

        // The dialog leaves a trailing \n character so this removes it
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
            result.pop_back();

        return std::filesystem::path(result);
    }

    void FileSystem::ShowFolder(std::filesystem::path path)
    {
#ifndef PX_PLATFORM_PSVITA
        int pid = fork();
        if (pid == 0) {
            std::string pathStr = path.string();
            int result = execl("/usr/bin/dolphin", "--select", pathStr.c_str(), (char *)0);
            if (result != 0)
                OpenFileWithDefaultProgram(path);
            exit(0);
        }
#endif
    }

    void FileSystem::OpenFileWithDefaultProgram(std::filesystem::path path)
    {
#ifndef PX_PLATFORM_PSVITA
        int pid = fork();
        if (pid == 0) {
            std::string pathStr = path.string();
            execl("/usr/bin/xdg-open", "xdg-open", pathStr.c_str(), (char *)0);
            exit(0);
        }
#endif
    }
}