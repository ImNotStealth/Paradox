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
		writer.Key("Windows");
		writer.StartObject();
		writer.Key("IconFilePath");
		writer.String(m_Properties.windowsProperties.iconFilePath.generic_string());
		writer.Key("BuildConfig");
		writer.String(m_Properties.windowsProperties.buildConfig);
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

		if (document["Windows"].HasMember("IconFilePath") && document["Windows"]["IconFilePath"].IsString())
			m_Properties.windowsProperties.iconFilePath = document["Windows"]["IconFilePath"].GetString();

		if (document["Windows"].HasMember("BuildConfig") && document["Windows"]["BuildConfig"].IsString())
			m_Properties.windowsProperties.buildConfig = document["Windows"]["BuildConfig"].GetString();

		return true;
	}

	void PlatformPackageConfig::WriteString(std::ostream& out, const std::string& str)
	{
		uint32_t size = (uint32_t)str.size();
		out.write((char*)&size, sizeof(uint32_t));
		out.write(str.c_str(), size);
	}

	std::string PlatformPackageConfig::ReadString(std::istream& in)
	{
		uint32_t size;
		in.read((char*)&size, sizeof(uint32_t));

		std::string str = std::string(size, '\0');
		in.read(str.data(), size);
		return str;
	}
}
