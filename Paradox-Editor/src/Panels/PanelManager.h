#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class PanelManager
	{
	public:
		void OnImGuiRender();
		void RenderMenuBar();
		void OnEvent(Event& event);

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
		std::vector<Unique<Panel>> m_Panels, m_Popups;
		std::unordered_map<std::string, bool> m_PanelStates;
		std::vector<std::string> m_QueuedPopups;
	};
}