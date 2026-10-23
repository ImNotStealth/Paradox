#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/AssetManager.h"

namespace Paradox
{
	struct ProjectProperties
	{
		std::string name = "Unnamed Project";
		std::filesystem::path path, assetPath;

		UUID startScene = UUID::EMPTY;
	};

	class Project
	{
	public:
		Project() = default;
		Project(ProjectProperties& properties);
		Project(const std::filesystem::path& filePath);

		bool Deserialize(const std::filesystem::path& filePath);
		void Serialize();

		ProjectProperties& GetProperties() { return m_Properties; }
		Shared<AssetManager> GetAssetManager() { return m_AssetManager; }

		inline static Shared<Project> GetActive() { return s_ActiveProject; }
		static void SetActive(Shared<Project> project);

	private:
		ProjectProperties m_Properties;
		Shared<AssetManager> m_AssetManager = nullptr;

		static Shared<Project> s_ActiveProject;
	};
}