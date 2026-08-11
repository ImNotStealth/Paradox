#include "AboutPanel.h"

#include <Paradox/Core/Base.h>
#include <Paradox/Core/Version.h>
#include <Paradox/Renderer/GraphicsContext.h>
#include <Paradox/ImGui/ImGuiUtils.h>

namespace Paradox
{
	void AboutPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(300.f, 300.f));

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		if (ImGui::BeginPopupModal(m_Name.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize))
		{
			ImGui::PopFont();
			ImVec2 popupSize = ImGui::GetWindowSize();
			ImVec2 logoSize = ImVec2(80.f, 80.f);
			ImVec2 logoPos = ImVec2((popupSize.x - logoSize.x) / 2.f, 35.f);
			ImGui::SetCursorPos(logoPos);
			ImGuiUtils::Image(m_LogoTexture, logoSize);

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::Text("Paradox Engine v%s", PX_VERSION);
			ImGui::PopFont();
			ImGui::Text("Environment: %s (%s)", PX_PLATFORM_NAME, PX_BUILD_CONFIG_NAME);
			ImGui::Text("Graphics API: %s", GraphicsContext::GraphicsAPIToString(GraphicsContext::GetGraphicsAPI()).c_str());

			ImGui::Dummy({ 0.f, 10.f });
			ImGui::Text("Made with <3 by Stealth");
			ImGui::TextLinkOpenURL("GitHub", "https://github.com/ImNotStealth/Paradox");

			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 buttonSize = ImVec2(80.f, 0.f);
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPos(ImVec2(windowSize.x / 2.f - (buttonSize.x / 2.f), windowSize.y - 36.f));
			if (ImGui::Button("Close", buttonSize))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
}
