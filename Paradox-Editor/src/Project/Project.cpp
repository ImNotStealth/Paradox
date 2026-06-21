#include "pxpch.h"
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
	Project Project::s_ActiveProject;

	Project::Project(const ProjectProperties& properties)
		: m_Properties(properties)
	{
		PX_INFO("Creating new Project: {0}", m_Properties.name);
		Serialize();
	}

	Project::Project(std::filesystem::path filePath)
	{
		PX_INFO("Loading Project from: {0}", filePath.string());
		Deserialize(filePath);
	}

	void Project::Deserialize(std::filesystem::path filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			PX_ERROR("Project file does not exist: {0}", filePath.string());
			return;
		}

		std::ifstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open Project file: {0}", filePath.string());
			return;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_ERROR("Failed to parse Project file: {0}", filePath.string());
			return;
		}

		if (!document.HasMember("FileVersion") || !document["FileVersion"].IsInt())
		{
			PX_ERROR("Project file is missing FileVersion: {0}", filePath.string());
			return;
		}

		uint8_t version = document["FileVersion"].GetInt();
		if (version != PROJECT_FILE_VERSION)
		{
			PX_ERROR("Project file version mismatch: {0} (expected {1})", version, PROJECT_FILE_VERSION);
			return;
		}

		m_Properties.name = filePath.stem().string();
		m_Properties.path = filePath.parent_path();
		m_Properties.assetPath = m_Properties.path / "Assets";
		PX_INFO("Loaded Project: {0}", m_Properties.name);
	}

	void Project::Serialize()
	{
		if (!std::filesystem::exists(m_Properties.path))
			std::filesystem::create_directory(m_Properties.path);

		if (!std::filesystem::exists(m_Properties.assetPath))
			std::filesystem::create_directory(m_Properties.assetPath);

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
		writer.String("FileVersion");
		writer.Int(PROJECT_FILE_VERSION);
		writer.EndObject();
		
		file << buffer.GetString();
		file.close();

		PX_INFO("Saved Project file: {0}", filePath.string());
	}

	void Project::SetActive(const Project& project)
	{
		s_ActiveProject = project;
		Application::Get().GetWindow().SetTitle("Paradox Editor - " + s_ActiveProject.m_Properties.name);
		ProjectChangedEvent event;
		Application::Get().BroadcastEvent(event);
	}
}