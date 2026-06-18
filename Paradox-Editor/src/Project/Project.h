#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct ProjectProperties
	{
		std::string name = "Unnamed Project";
		std::filesystem::path path;
	};

	class Project
	{
	public:
		Project() = default;
		Project(const ProjectProperties& properties);
		Project(std::filesystem::path filePath);

		void Deserialize(std::filesystem::path filePath);
		void Serialize();

		const ProjectProperties& GetProperties() const { return m_Properties; }

		inline static const Project& GetActive() { return s_ActiveProject; }
		static void SetActive(const Project& project);

	private:
		ProjectProperties m_Properties;

		static Project s_ActiveProject;
	};
}