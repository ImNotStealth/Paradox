#include "PlatformPackageConfig.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define PACKAGE_CONFIG_FILE_VERSION 1

namespace Paradox
{
	void PlatformPackageConfig::Serialize(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
			std::filesystem::create_directories(filePath.parent_path());

		std::ofstream file(filePath.string());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open PlatformPackageConfig file: {0}", filePath.string());
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(PACKAGE_CONFIG_FILE_VERSION);

		writer.Key("BuildConfig");
		writer.String(m_Properties.buildConfig);
		writer.Key("FullRebuild");
		writer.Bool(m_Properties.fullRebuild);
		writer.Key("BuildDLL");
		writer.Bool(m_Properties.buildDll);
		writer.Key("IncludeImGui");
		writer.Bool(m_Properties.includeImGui);
		writer.Key("IncludeTracy");
		writer.Bool(m_Properties.includeTracy);
		writer.Key("IncludeSpdLog");
		writer.Bool(m_Properties.includeSpdLog);

		writer.Key("Windows");
		writer.StartObject();
		writer.Key("IconFilePath");
		writer.String(m_Properties.windowsProperties.iconFilePath.generic_string());
		writer.Key("DefaultGraphicsAPI");
		writer.String(GraphicsContext::GraphicsAPIToString(m_Properties.windowsProperties.defaultGraphicsAPI));
		writer.Key("IncludeVulkan");
		writer.Bool(m_Properties.windowsProperties.includeVulkan);
		writer.Key("IncludeOpenGL");
		writer.Bool(m_Properties.windowsProperties.includeOpenGL);
		writer.EndObject();

		writer.Key("Linux");
		writer.StartObject();
		writer.Key("DefaultGraphicsAPI");
		writer.String(GraphicsContext::GraphicsAPIToString(m_Properties.linuxProperties.defaultGraphicsAPI));
		writer.Key("IncludeVulkan");
		writer.Bool(m_Properties.linuxProperties.includeVulkan);
		writer.Key("IncludeOpenGL");
		writer.Bool(m_Properties.linuxProperties.includeOpenGL);
		writer.EndObject();

		writer.EndObject();

		file << buffer.GetString();
		file.close();

		PX_INFO("Saved PlatformPackageConfig file: {0}", filePath.string());
	}

	bool PlatformPackageConfig::Deserialize(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			PX_ERROR("PlatformPackageConfig file does not exist: {0}", filePath.string());
			return false;
		}

		std::ifstream file(filePath.string());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open PlatformPackageConfig file: {0}", filePath.string());
			return false;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_ERROR("Failed to parse PlatformPackageConfig file: {0}", filePath.string());
			return false;
		}

		if (!document.HasMember("FileVersion") || !document["FileVersion"].IsInt())
		{
			PX_ERROR("PlatformPackageConfig file is missing FileVersion: {0}", filePath.string());
			return false;
		}

		uint8_t version = document["FileVersion"].GetInt();
		if (version != PACKAGE_CONFIG_FILE_VERSION)
		{
			PX_ERROR("PlatformPackageConfig file version mismatch: {0} (expected {1})", version, PACKAGE_CONFIG_FILE_VERSION);
			return false;
		}

		if (!document.HasMember("Windows"))
		{
			PX_ERROR("PlatformPackageConfig file is missing Windows: {0}", filePath.string());
			return false;
		}

		if (!document.HasMember("Linux"))
		{
			PX_ERROR("PlatformPackageConfig file is missing Linux: {0}", filePath.string());
			return false;
		}

		if (document.HasMember("BuildConfig") && document["BuildConfig"].IsString())
			m_Properties.buildConfig = document["BuildConfig"].GetString();

		if (document.HasMember("FullRebuild") && document["FullRebuild"].IsBool())
			m_Properties.fullRebuild = document["FullRebuild"].GetBool();

		if (document.HasMember("BuildDLL") && document["BuildDLL"].IsBool())
			m_Properties.buildDll = document["BuildDLL"].GetBool();

		if (document.HasMember("IncludeImGui") && document["IncludeImGui"].IsBool())
			m_Properties.includeImGui = document["IncludeImGui"].GetBool();

		if (document.HasMember("IncludeTracy") && document["IncludeTracy"].IsBool())
			m_Properties.includeTracy = document["IncludeTracy"].GetBool();

		if (document.HasMember("IncludeSpdLog") && document["IncludeSpdLog"].IsBool())
			m_Properties.includeSpdLog = document["IncludeSpdLog"].GetBool();

		// Windows
		if (document["Windows"].HasMember("IconFilePath") && document["Windows"]["IconFilePath"].IsString())
			m_Properties.windowsProperties.iconFilePath = document["Windows"]["IconFilePath"].GetString();

		if (document["Windows"].HasMember("DefaultGraphicsAPI") && document["Windows"]["DefaultGraphicsAPI"].IsString())
			m_Properties.windowsProperties.defaultGraphicsAPI = GraphicsContext::StringToGraphicsAPI(document["Windows"]["DefaultGraphicsAPI"].GetString());

		if (document["Windows"].HasMember("IncludeVulkan") && document["Windows"]["IncludeVulkan"].IsBool())
			m_Properties.windowsProperties.includeVulkan = document["Windows"]["IncludeVulkan"].GetBool();

		if (document["Windows"].HasMember("IncludeOpenGL") && document["Windows"]["IncludeOpenGL"].IsBool())
			m_Properties.windowsProperties.includeOpenGL = document["Windows"]["IncludeOpenGL"].GetBool();

		// Linux
		if (document["Linux"].HasMember("DefaultGraphicsAPI") && document["Linux"]["DefaultGraphicsAPI"].IsString())
			m_Properties.linuxProperties.defaultGraphicsAPI = GraphicsContext::StringToGraphicsAPI(document["Linux"]["DefaultGraphicsAPI"].GetString());

		if (document["Linux"].HasMember("IncludeVulkan") && document["Linux"]["IncludeVulkan"].IsBool())
			m_Properties.linuxProperties.includeVulkan = document["Linux"]["IncludeVulkan"].GetBool();

		if (document["Linux"].HasMember("IncludeOpenGL") && document["Linux"]["IncludeOpenGL"].IsBool())
			m_Properties.linuxProperties.includeOpenGL = document["Linux"]["IncludeOpenGL"].GetBool();

		return true;
	}
}
