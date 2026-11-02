#include "EditorConfig.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define EDITOR_CONFIG_FILE_VERSION 1

namespace Paradox
{
	EditorConfigProperties EditorConfig::m_Properties = GetDefaultProperties();

	void EditorConfig::Serialize()
	{
		const std::filesystem::path filePath = std::filesystem::current_path() / "Config" / "EditorSettings.json";

		if (!std::filesystem::exists(filePath))
			std::filesystem::create_directories(filePath.parent_path());

		std::ofstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open EditorConfig file: {0}", filePath.filename().string());
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(EDITOR_CONFIG_FILE_VERSION);
		writer.Key("EngineSourcePath");
		writer.String(m_Properties.engineSourcePath.generic_string());
		writer.Key("OpenPackagedFolder");
		writer.Bool(m_Properties.openPackagedFolder);
		writer.EndObject();

		file << buffer.GetString();
		file.close();

		PX_INFO("Saved EditorConfig file: {0}", filePath.string());
	}

	void EditorConfig::Deserialize()
	{
		const std::filesystem::path filePath = std::filesystem::current_path() / "Config" / "EditorSettings.json";

		if (!std::filesystem::exists(filePath))
		{
			Serialize();
			return;
		}

		std::ifstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open EditorConfig file: {0}", filePath.string());
			return;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_ERROR("Failed to parse EditorConfig file: {0}", filePath.string());
			return;
		}

		if (!document.HasMember("FileVersion") || !document["FileVersion"].IsInt())
		{
			PX_ERROR("EditorConfig file is missing FileVersion: {0}", filePath.string());
			return;
		}

		uint8_t version = document["FileVersion"].GetInt();
		if (version != EDITOR_CONFIG_FILE_VERSION)
		{
			PX_ERROR("EditorConfig file version mismatch: {0} (expected {1})", version, EDITOR_CONFIG_FILE_VERSION);
			return;
		}

		if (document.HasMember("EngineSourcePath") && document["EngineSourcePath"].IsString())
			m_Properties.engineSourcePath = document["EngineSourcePath"].GetString();

		if (document.HasMember("OpenPackagedFolder") && document["OpenPackagedFolder"].IsBool())
			m_Properties.openPackagedFolder = document["OpenPackagedFolder"].GetBool();
	}

	EditorConfigProperties Paradox::EditorConfig::GetDefaultProperties()
	{
		EditorConfigProperties props;

		// Goes up 2 folders to main Paradox source folder
		props.engineSourcePath = std::filesystem::current_path().parent_path().parent_path();
		props.openPackagedFolder = true;

		return props;
	}
}