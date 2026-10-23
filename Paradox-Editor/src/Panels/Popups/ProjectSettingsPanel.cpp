#include "ProjectSettingsPanel.h"

#include "Project/Project.h"

#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
	void ProjectSettingsPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();
		if (ImGui::BeginPopupModal(m_Name.c_str(), opened))
		{
			if (!Project::GetActive())
			{
				ImGui::TextUnformatted("Open a Project to start.");
				ImGui::EndPopup();
				return;
			}

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1], ImGui::GetFontSize() * 1.2f);
			ImGui::TextUnformatted("Runtime");
			ImGui::PopFont();
			ImGui::Separator();
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::TextUnformatted("Start Scene");
			ImGui::PopFont();
			std::string sceneUuid = Project::GetActive()->GetProperties().startScene.ToString();
			if (ImGui::InputText("##SceneUUID", &sceneUuid))
				Project::GetActive()->GetProperties().startScene = UUID(sceneUuid);

			if (ImGui::Button("Clear Start Scene"))
				Project::GetActive()->GetProperties().startScene = UUID::EMPTY;

			if (ImGui::Button("Save"))
				Project::GetActive()->Serialize();

			ImGui::EndPopup();
		}
	}
}
