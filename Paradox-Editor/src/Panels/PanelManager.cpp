#include "pxpch.h"
#include "PanelManager.h"

namespace Paradox
{
	void PanelManager::OnImGuiRender()
	{
		for (const auto& panel : m_Panels)
		{
			bool& opened = m_PanelStates[panel->GetName()];
			if (opened)
				panel->OnImGuiRender(&opened);
		}

		for (const std::string& popupName : m_QueuedPopups)
			ImGui::OpenPopup(popupName.c_str());
		m_QueuedPopups.clear();

		for (const auto& panel : m_Popups)
			panel->OnImGuiRender(nullptr);

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