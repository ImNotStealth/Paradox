#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/Window.h"
#include "Paradox/Scene/UUID.h"

namespace Paradox
{
	struct WindowsPackageConfigProperties
	{
		std::filesystem::path iconFilePath;
	};

	struct PlatformPackageConfigProperties
	{
		WindowsPackageConfigProperties windowsProperties;
	};

	class PlatformPackageConfig
	{
	public:
		PlatformPackageConfig() = default;
		PlatformPackageConfig(PlatformPackageConfigProperties& properties)
			: m_Properties(properties) {}

		void Serialize(const std::filesystem::path& filePath);
		bool Deserialize(const std::filesystem::path& filePath);

		PlatformPackageConfigProperties& GetProperties() { return m_Properties; }

	private:
		//TODO: Should move this to a generic file
		void WriteString(std::ostream& out, const std::string& str);
		std::string ReadString(std::istream& in);

	private:
		PlatformPackageConfigProperties m_Properties;
	};
}