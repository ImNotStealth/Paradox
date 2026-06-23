#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Shared<Texture> Create(const std::string& debugName, const std::filesystem::path& filePath);
	};
}