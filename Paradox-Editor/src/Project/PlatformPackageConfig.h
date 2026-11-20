#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/GraphicsContext.h"
#include "Paradox/Scene/UUID.h"

namespace Paradox
{
	struct WindowsPackageConfigProperties
	{
		std::filesystem::path iconFilePath;
		GraphicsAPIType defaultGraphicsAPI = GraphicsAPIType::Vulkan;
		bool includeVulkan = true, includeOpenGL = true;
	};

	struct LinuxPackageConfigProperties
	{
		GraphicsAPIType defaultGraphicsAPI = GraphicsAPIType::Vulkan;
		bool includeVulkan = true, includeOpenGL = true;
	};

	struct VitaPackageConfigProperties
	{
		std::string titleID = "PRDX00001";
		UUID splashTexture = UUID::EMPTY;
		UUID bubbleIconTexture = UUID::EMPTY;
		UUID backgroundTexture = UUID::EMPTY;
		UUID launchTexture = UUID::EMPTY;
	};

	struct PlatformPackageConfigProperties
	{
		std::string buildConfig = "Dist";
		bool fullRebuild = false;
		bool buildDll = true;
		bool includeImGui = false, includeTracy = false, includeSpdLog = false;
		WindowsPackageConfigProperties windowsProperties;
		LinuxPackageConfigProperties linuxProperties;
		VitaPackageConfigProperties vitaProperties;
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
