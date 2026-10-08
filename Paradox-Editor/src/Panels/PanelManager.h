#pragma once

#include "Panels/Panel.h"
#include "Panels/AssetEditors/AssetEditorPanel.h"

namespace Paradox
{
	class PanelManager
	{
	public:
		void RegisterPanels();

		void OnImGuiRender(float deltaTime);
		void RenderMenuBar();
		void OnEvent(Event& event);

		void OpenAssetEditor(Shared<AssetMetadata> metadata)
		{
			for (auto& [editorPanel, opened] : m_AssetEditors)
			{
				if (editorPanel->GetMetadata() == metadata)
				{
					PX_WARN("AssetEditor Panel is already open for: {0}", metadata->GetUUID().ToString());
					return;
				}
			}

			Unique<AssetEditorPanel> editorPanel = m_EditorFactories[metadata->GetAssetType()](metadata);
			PX_INFO("Opened AssetEditor: {0}", editorPanel->GetName());
			m_AssetEditors[std::move(editorPanel)] = true;
		}

		template<typename T, typename... Args>
		void OpenPopup(Args&&... args)
		{
			bool isDerived = std::is_base_of<Panel, T>::value;
			PX_ASSERT(isDerived, "T must be derived from Panel");

			Unique<Panel> panel = CreateUnique<T>(std::forward<Args>(args)...);
			PX_INFO("Opened Popup: {0}", panel->GetName());
			m_QueuedPopups.emplace_back(panel->GetName());
			m_Popups.emplace_back(std::move(panel));
		}

	private:
		template<typename T, typename... Args>
		void RegisterPanel(bool openByDefault, Args&&... args)
		{
			bool isDerived = std::is_base_of<Panel, T>::value;
			PX_ASSERT(isDerived, "T must be derived from Panel");

			Unique<Panel> panel = CreateUnique<T>(std::forward<Args>(args)...);
			PX_INFO("Registered Panel: {0}", panel->GetName());
			m_PanelStates[panel->GetName()] = openByDefault;
			m_Panels.emplace_back(std::move(panel));
		}

		template<typename T>
		void RegisterAssetEditorType(AssetType type)
		{
			bool isDerived = std::is_base_of<AssetEditorPanel, T>::value;
			PX_ASSERT(isDerived, "T must be derived from Panel");

			PX_ASSERT(m_EditorFactories.find(type) == m_EditorFactories.end(), "Duplicate AssetType.");
			m_EditorFactories[type] = [type](Shared<AssetMetadata> meta) { return CreateUnique<T>(meta); };
		}

	private:
		std::vector<Unique<Panel>> m_Panels, m_Popups;
		std::unordered_map<std::string, bool> m_PanelStates;
		std::vector<std::string> m_QueuedPopups;

		std::unordered_map<Unique<AssetEditorPanel>, bool> m_AssetEditors;
		std::unordered_map<AssetType, std::function<Unique<AssetEditorPanel>(Shared<AssetMetadata>)>> m_EditorFactories;
	};
}