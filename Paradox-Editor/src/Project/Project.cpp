#include "Project.h"

#include "Events/EditorEvents.h"

#include <Paradox/Core/Application.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define PROJECT_FILE_VERSION 1

namespace Paradox
{
	Shared<Project> Project::s_ActiveProject;

	Project::Project(ProjectProperties& properties)
		: m_Properties(properties)
	{
		PX_INFO("Creating new Project: {0}", m_Properties.name);
		Serialize();
		m_AssetManager = CreateShared<AssetManager>(m_Properties.assetPath);
		AssetManager::SetInstance(m_AssetManager.get());
	}

	Project::Project(const std::filesystem::path& filePath)
	{
		PX_INFO("Loading Project from: {0}", filePath.string());
		if (!Deserialize(filePath))
			return;

		m_AssetManager = CreateShared<AssetManager>(m_Properties.assetPath);
		AssetManager::SetInstance(m_AssetManager.get());
	}

	bool Project::Deserialize(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			PX_ERROR("Project file does not exist: {0}", filePath.string());
			return false;
		}

		std::ifstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open Project file: {0}", filePath.string());
			return false;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_ERROR("Failed to parse Project file: {0}", filePath.string());
			return false;
		}

		if (!document.HasMember("FileVersion") || !document["FileVersion"].IsInt())
		{
			PX_ERROR("Project file is missing FileVersion: {0}", filePath.string());
			return false;
		}

		uint8_t version = document["FileVersion"].GetInt();
		if (version != PROJECT_FILE_VERSION)
		{
			PX_ERROR("Project file version mismatch: {0} (expected {1})", version, PROJECT_FILE_VERSION);
			return false;
		}

		if (!document.HasMember("AssetPath") || !document["AssetPath"].IsString())
		{
			PX_ERROR("Project file is missing AssetPath: {0}", filePath.string());
			return false;
		}

		if (!document.HasMember("StartScene") || !document["StartScene"].IsString())
		{
			PX_ERROR("Project file is missing StartScene: {0}", filePath.string());
			return false;
		}

		m_Properties.name = filePath.stem().string();
		m_Properties.path = filePath.parent_path();
		m_Properties.assetPath = m_Properties.path / document["AssetPath"].GetString();
		m_Properties.startScene = UUID::IsValid(document["StartScene"].GetString()) ? UUID(document["StartScene"].GetString()) : UUID::EMPTY;
		PX_INFO("Loaded Project: {0}", m_Properties.name);
		return true;
	}

	void Project::Serialize()
	{
		if (!std::filesystem::exists(m_Properties.path))
			std::filesystem::create_directories(m_Properties.path.parent_path());

		if (!std::filesystem::exists(m_Properties.assetPath))
			std::filesystem::create_directories(m_Properties.assetPath);

		std::string fileName = m_Properties.name + ".px";
		std::filesystem::path filePath = m_Properties.path / fileName;

		std::ofstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open Project file: {0}", fileName);
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(PROJECT_FILE_VERSION);
		writer.Key("AssetPath");
		writer.String(m_Properties.assetPath.stem().string());
		writer.Key("StartScene");
		writer.String(m_Properties.startScene.ToString());
		writer.EndObject();
		
		file << buffer.GetString();
		file.close();

		PX_INFO("Saved Project file: {0}", filePath.string());
	}

	void Project::SetActive(Shared<Project> project)
	{
		s_ActiveProject = project;

		if (s_ActiveProject)
		{
			ProjectChangedEvent event;
			Application::Get().BroadcastEvent(event);
		}
	}
}