#pragma once

#include "Assets/EditorAssetManager.h"

namespace Paradox
{
	struct ProjectProperties
	{
		std::string name = "Unnamed Project";
		std::filesystem::path path, assetPath;

		std::string version = "1.0";
		std::string author = "None";
		std::string copyright = "Fill in your Copyright in Project Settings";

		UUID startScene = UUID::EMPTY;
	};

	class Project
	{
	public:
		Project() = default;
		Project(ProjectProperties& properties);
		Project(const std::filesystem::path& filePath);

		void Serialize();
		bool Deserialize(const std::filesystem::path& filePath);

		ProjectProperties& GetProperties() { return m_Properties; }
		Shared<EditorAssetManager> GetAssetManager() { return m_AssetManager; }

		inline static Shared<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Shared<Project> project);

	private:
		ProjectProperties m_Properties;
		Shared<EditorAssetManager> m_AssetManager = nullptr;

		static Shared<Project> s_ActiveProject;
	};
}