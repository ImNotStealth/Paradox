#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API FileSystem
	{
	public:
		static std::filesystem::path SelectFile(const std::string& title, const std::string& filter);
		static std::filesystem::path SelectFolder(const std::string& title);
		static void ShowFolder(std::filesystem::path path);
		static void OpenFileWithDefaultProgram(std::filesystem::path path);
	};
}