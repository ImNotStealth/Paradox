#include "EditorConfig.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define EDITOR_CONFIG_FILE_VERSION 1

namespace Paradox
{
	EditorConfigProperties EditorConfig::s_Properties = GetDefaultProperties();

	void EditorConfig::Serialize()
	{
		const std::filesystem::path filePath = std::filesystem::current_path() / "Config" / "EditorSettings.json";

		if (!std::filesystem::exists(filePath))
			std::filesystem::create_directories(filePath.parent_path());

		std::ofstream file(filePath.string());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open EditorConfig file: {0}", filePath.filename().string());
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);
		writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(EDITOR_CONFIG_FILE_VERSION);
		writer.Key("EngineSourcePath");
		writer.String(s_Properties.engineSourcePath.generic_string());
		writer.Key("CMakeGenerator");
		writer.String(s_Properties.cmakeGenerator);
		writer.Key("OpenPackagedFolder");
		writer.Bool(s_Properties.openPackagedFolder);

		writer.Key("GraphicsAPI");
		writer.String(GraphicsContext::GraphicsAPIToString(s_Properties.graphicsApi));
		writer.Key("VSync");

		writer.Bool(s_Properties.vsync);
		writer.Key("DrawGrid");
		writer.Bool(s_Properties.drawGrid);
		
		writer.Key("GridColor");
		writer.StartArray();
		writer.Double(s_Properties.gridColor.r);
		writer.Double(s_Properties.gridColor.g);
		writer.Double(s_Properties.gridColor.b);
		writer.Double(s_Properties.gridColor.a);
		writer.EndArray();
		
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

		std::ifstream file(filePath.string());
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
			s_Properties.engineSourcePath = document["EngineSourcePath"].GetString();

		if (document.HasMember("CMakeGenerator") && document["CMakeGenerator"].IsString())
			s_Properties.cmakeGenerator = document["CMakeGenerator"].GetString();

		if (document.HasMember("OpenPackagedFolder") && document["OpenPackagedFolder"].IsBool())
			s_Properties.openPackagedFolder = document["OpenPackagedFolder"].GetBool();

		if (document.HasMember("GraphicsAPI") && document["GraphicsAPI"].IsString())
			s_Properties.graphicsApi = GraphicsContext::StringToGraphicsAPI(document["GraphicsAPI"].GetString());

		if (document.HasMember("VSync") && document["VSync"].IsBool())
			s_Properties.vsync = document["VSync"].GetBool();

		if (document.HasMember("DrawGrid") && document["DrawGrid"].IsBool())
			s_Properties.drawGrid = document["DrawGrid"].GetBool();

		if (document.HasMember("GridColor"))
		{
			const rapidjson::Value& gridColorVal = document["GridColor"];
			if (document["DrawGrid"].IsArray())
				s_Properties.gridColor = { gridColorVal["Color"][0].GetFloat(), gridColorVal["Color"][1].GetFloat(), gridColorVal["Color"][2].GetFloat(), gridColorVal["Color"][3].GetFloat() };
		}
	}

	EditorConfigProperties Paradox::EditorConfig::GetDefaultProperties()
	{
		EditorConfigProperties props;

		// Goes up 2 folders to main Paradox source folder
		props.engineSourcePath = std::filesystem::current_path().parent_path().parent_path();
		props.openPackagedFolder = true;
#ifdef PX_PLATFORM_WINDOWS
		props.cmakeGenerator = "Visual Studio 18 2026";
#elif defined(PX_PLATFORM_LINUX)
		props.cmakeGenerator = "Ninja";
#endif
		props.graphicsApi = GraphicsAPIType::Vulkan;
		props.vsync = true;

		props.drawGrid = true;
		props.gridColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.5f);
		props.gridScale = 100.f;
		props.gridSize = 0.02f;

		return props;
	}
}