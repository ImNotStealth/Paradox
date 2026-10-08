#include "PanelManager.h"

#include "Panels/ConsoleLogPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/StatisticsPanel.h"
#include "Panels/SceneTreePanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/AssetIndexPanel.h"
#include "Panels/AssetEditors/Texture2DAssetEditor.h"

namespace Paradox
{
	void PanelManager::RegisterPanels()
	{
		RegisterPanel<ConsoleLogPanel>(true);
		RegisterPanel<AssetBrowserPanel>(true);
		RegisterPanel<StatisticsPanel>(false);
		RegisterPanel<SceneTreePanel>(true);
		RegisterPanel<InspectorPanel>(true);
		RegisterPanel<AssetIndexPanel>(false);

		RegisterAssetEditorType<Texture2DAssetEditor>(AssetType::Texture2D);
	}

	void PanelManager::OnImGuiRender(float deltaTime)
	{
		for (const auto& panel : m_Panels)
		{
			bool& opened = m_PanelStates[panel->GetName()];
			if (opened)
				panel->OnImGuiRender(&opened, deltaTime);
		}

		for (auto it = m_AssetEditors.begin(); it != m_AssetEditors.end();)
		{
			auto& [editorPanel, opened] = *it;
			if (opened)
				editorPanel->OnImGuiRender(&opened, deltaTime);

			if (!opened)
			{
				PX_INFO("Closed AssetEditor: {0}", editorPanel->GetName());
				editorPanel->OnClose();
				it = m_AssetEditors.erase(it);
			}
			else
				++it;
		}

		for (const std::string& popupName : m_QueuedPopups)
			ImGui::OpenPopup(popupName.c_str());
		m_QueuedPopups.clear();

		for (const auto& popup : m_Popups)
			popup->OnImGuiRender(nullptr, deltaTime);

		m_Popups.erase(std::remove_if(m_Popups.begin(), m_Popups.end(),
			[](const auto& panel) { return !ImGui::IsPopupOpen(panel->GetName().c_str()); }), m_Popups.end());
	}

	void PanelManager::RenderMenuBar()
	{
		if (ImGui::BeginMenu("Window"))
		{
			for (const auto& panel : m_Panels)
				ImGui::MenuItem(panel->GetName().c_str(), nullptr, &m_PanelStates[panel->GetName()]);

			ImGui::EndMenu();
		}
	}

	void PanelManager::OnEvent(Event& event)
	{
		for (const auto& panel : m_Panels)
		{
			panel->OnEvent(event);
			if (event.Handled())
				break;
		}

		for (const auto& panel : m_Popups)
		{
			panel->OnEvent(event);
			if (event.Handled())
				break;
		}
	}
}