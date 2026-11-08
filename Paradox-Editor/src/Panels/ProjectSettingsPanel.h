#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class ProjectSettingsPanel : public Panel
	{
	public:
		ProjectSettingsPanel();

		void OnImGuiRender(bool* opened, float deltaTime) override;
		void OnClose() override;

	private:
		void DrawDescriptionSettings();
		void DrawRuntimeSettings();

	private:
		std::unordered_map<std::string, std::function<void()>> m_SettingsCategories;
		std::string m_SelectedCategory;
	};
}