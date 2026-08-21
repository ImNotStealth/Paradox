#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class CreateProjectPanel : public Panel
	{
	public:
		CreateProjectPanel()
			: Panel("Create Project")
		{
			m_ProjectPath = std::filesystem::current_path();
		}

		void OnImGuiRender(bool* opened) override;

	private:
		bool IsNameValid(const std::string& name);

	private:
		std::string m_ProjectName = "Unnamed Project";
		std::filesystem::path m_ProjectPath;
		std::string m_ErrorMessage;
	};
}