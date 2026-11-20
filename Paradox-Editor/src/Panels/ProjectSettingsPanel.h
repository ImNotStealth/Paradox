#pragma once

#include "Panels/Panel.h"

#include <Paradox.h>

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
		void DrawLinuxSettings();
		void DrawVitaSettings();

	private:
		class EditorApp& m_AppRef;
		std::map<std::string, std::function<void()>> m_SettingsCategories;
		std::string m_SelectedCategory;

		Reference<Texture2D> m_VitaSplashTexture = nullptr;
		Reference<Texture2D> m_VitaBubbleTexture = nullptr;
		Reference<Texture2D> m_VitaBackgroundTexture = nullptr;
		Reference<Texture2D> m_VitaLaunchTexture = nullptr;
	};
}