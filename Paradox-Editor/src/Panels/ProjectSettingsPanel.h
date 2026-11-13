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
		void DrawPackageSettings();
		void DrawWindowsSettings();

	private:
		class EditorApp& m_AppRef;
		std::map<std::string, std::function<void()>> m_SettingsCategories;
		std::string m_SelectedCategory;
	};
}