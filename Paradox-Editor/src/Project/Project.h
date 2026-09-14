#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/AssetManager.h"

namespace Paradox
{
	struct ProjectProperties
	{
		std::string name = "Unnamed Project";
		std::filesystem::path path, assetPath;
	};

	class Project
	{
	public:
		Project() = default;
		Project(const ProjectProperties& properties);
		Project(const std::filesystem::path& filePath);

		void Deserialize(const std::filesystem::path& filePath);
		void Serialize();

		const ProjectProperties& GetProperties() const { return m_Properties; }
		Shared<AssetManager> GetAssetManager() { return m_AssetManager; }

		inline static const Project& GetActive() { return s_ActiveProject; }
		static void SetActive(const Project& project);

	private:
		ProjectProperties m_Properties;
		Shared<AssetManager> m_AssetManager = nullptr;

		static Project s_ActiveProject;
	};
}