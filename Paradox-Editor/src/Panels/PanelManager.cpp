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
		RegisterPanel<ConsoleLogPanel>(true,	"General");
		RegisterPanel<AssetBrowserPanel>(true,	"General");
		RegisterPanel<SceneTreePanel>(true,		"General");
		RegisterPanel<InspectorPanel>(true,		"General");
		RegisterPanel<StatisticsPanel>(false,	"Debugging");
		RegisterPanel<AssetIndexPanel>(false,	"Debugging");

		RegisterAssetEditorType<Texture2DAssetEditor>(AssetType::Texture2D);
	}

	void PanelManager::OnImGuiRender(float deltaTime)
	{
		// Panels that stay alive even if they aren't open
		for (auto& entry : m_Panels)
		{
			bool oldOpen = entry.opened;
			if (entry.opened)
				entry.panel->OnImGuiRender(&entry.opened, deltaTime);
			if (!entry.opened && oldOpen)
				entry.panel->OnClose();
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

		for (auto it = m_Popups.begin(); it != m_Popups.end();)
		{
			auto& [panel, opened] = *it;
			if (opened)
				panel->OnImGuiRender(&opened, deltaTime);

			if (!opened)
			{
				PX_INFO("Closed Popup: {0}", panel->GetName());
				panel->OnClose();
				it = m_Popups.erase(it);
			}
			else
				++it;
		}
	}

	void PanelManager::RenderMenuBar()
	{
		if (ImGui::BeginMenu("Window"))
		{
			std::string oldCategory;
			for (auto& entry : m_Panels)
			{
				if (entry.category != oldCategory)
				{
					ImGui::SeparatorText(entry.category.c_str());
					oldCategory = entry.category;
				}

				ImGui::MenuItem(entry.panel->GetName().c_str(), nullptr, &entry.opened);
			}

			ImGui::EndMenu();
		}
	}

	void PanelManager::OnEvent(Event& event)
	{
		for (const auto& entry : m_Panels)
		{
			entry.panel->OnEvent(event);
			if (event.Handled())
				break;
		}

		for (const auto& [panel, opened] : m_Popups)
		{
			panel->OnEvent(event);
			if (event.Handled())
				break;
		}
	}
}