#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct ProjectProperties
	{
		std::string name = "Unnamed Project";
	};

	class Project
	{
	public:
		Project() = default;
		Project(std::filesystem::path filePath);

		void Deserialize(std::filesystem::path filePath);
		void Serialize();

		const ProjectProperties& GetProperties() const { return m_Properties; }

	private:
		ProjectProperties m_Properties;
	};
}