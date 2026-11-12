#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class EditorSettingsPanel : public Panel
	{
	public:
		EditorSettingsPanel();

		void OnImGuiRender(bool* opened, float deltaTime) override;
		void OnClose() override;

	private:
		void DrawRendererSettings();
		void DrawPackageSettings();
		void DrawViewportSettings();

	private:
		class EditorApp& m_AppRef;
		std::unordered_map<std::string, std::function<void()>> m_SettingsCategories;
		std::string m_SelectedCategory;
	};
}