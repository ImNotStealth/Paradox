#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API FileDialog
	{
	public:
		static std::filesystem::path SelectFolder(const std::string& title);
	};
}