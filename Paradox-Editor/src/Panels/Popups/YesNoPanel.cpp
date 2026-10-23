#include "YesNoPanel.h"

namespace Paradox
{
	void YesNoPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();
		if (ImGui::BeginPopupModal(m_Name.c_str(), opened, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextUnformatted(m_Message.c_str());

			const ImGuiStyle& style = ImGui::GetStyle();
			const float confirmWidth = ImGui::CalcTextSize(m_Confirm.c_str()).x + style.FramePadding.x * 2.0f;
			const float cancelWidth = ImGui::CalcTextSize(m_Cancel.c_str()).x + style.FramePadding.x * 2.0f;
			const float totalWidth = confirmWidth + style.ItemSpacing.x + cancelWidth;
			const float offset = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

			ImGui::Dummy({ 0.f, 10.f });

			if (offset > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

			if (ImGui::Button(m_Confirm.c_str()))
			{
				m_Callback(true);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button(m_Cancel.c_str()))
			{
				m_Callback(false);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
