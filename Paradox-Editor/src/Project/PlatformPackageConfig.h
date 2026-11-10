#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct WindowsPackageConfigProperties
	{
		std::filesystem::path iconFilePath;
		std::string buildConfig = "Dist";
		bool fullRebuild = false;
		bool buildDll = true;
		bool includeImGui = false, includeTracy = false, includeSpdLog = false;
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
		PlatformPackageConfigProperties m_Properties;
	};
}