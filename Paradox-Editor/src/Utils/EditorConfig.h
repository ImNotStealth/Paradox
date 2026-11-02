#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct EditorConfigProperties
	{
		//std::filesystem::path 
	};

	class EditorConfig
	{
		static void Serialize(const std::filesystem::path& filePath);
		static void Deserialize(const std::filesystem::path& filePath);
	};
}