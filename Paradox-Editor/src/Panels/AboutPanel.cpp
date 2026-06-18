#include "AboutPanel.h"

#include <Paradox/Core/Base.h>
#include <Paradox/Core/Version.h>
#include <Paradox/Renderer/GraphicsContext.h>

namespace Paradox
{
	void AboutPanel::OnImGuiRender()
	{
		ImGui::SetNextWindowSize(ImVec2(300.f, 200.f));
		if (ImGui::BeginPopupModal(m_Name.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Paradox Engine v%s", PX_VERSION);
			ImGui::Text("Environment: %s (%s)", PX_PLATFORM_NAME, PX_BUILD_CONFIG_NAME);
			ImGui::Text("Graphics API: %s", GraphicsContext::GraphicsAPIToString(GraphicsContext::GetGraphicsAPI()).c_str());

			ImGui::Dummy({ 0.f, 10.f });
			ImGui::Text("Made with <3");
			ImGui::TextLinkOpenURL("https://github.com/ImNotStealth/Paradox");

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
