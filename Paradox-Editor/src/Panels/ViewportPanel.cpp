#include "pxpch.h"
#include "ViewportPanel.h"

#include "EditorApp.h"
#include "Utils/EditorIcons.h"

#include <Paradox.h>
#include <Paradox/ImGui/ImGuiUtils.h>
#include <imgui.h>
#include <ImGuizmo.h>

namespace Paradox
{
	ViewportPanel::ViewportPanel()
		: Panel("Viewport"), m_AppRef((EditorApp&)Application::Get()) {}

	void ViewportPanel::OnImGuiRender(bool* opened)
	{
		PX_PROFILE_FUNCTION();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImGuizmo::SetDrawlist();

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		ImVec2 uv0, uv1;
		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::OpenGL)
		{
			uv0 = ImVec2(0, 1);
			uv1 = ImVec2(1, 0);
		}
		else
		{
			uv0 = ImVec2(0, 0);
			uv1 = ImVec2(1, 1);
		}

		ImGuiUtils::Image(m_AppRef.GetCompositeFramebuffer()->GetAttachmentImage(0), viewportPanelSize, {1, 1, 1, 1}, uv0, uv1);

		ImGui::SetCursorScreenPos({ m_ViewportBounds[1].x - 16.f - 12.f - ImGui::GetStyle().ItemSpacing.x, m_ViewportBounds[0].y + ImGui::GetStyle().ItemSpacing.x });
		if (ImGuiUtils::IconButton("ViewportSettings", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_SETTINGS)), { 16.f, 16.f }))
			ImGui::OpenPopup("ViewportSettingsPopup");

		ImVec2 buttonMin = ImGui::GetItemRectMin();
		ImVec2 buttonMax = ImGui::GetItemRectMax();

		ImVec2 padding = ImGui::GetStyle().WindowPadding;
		ImGui::PopStyleVar();
		ImGui::SetNextWindowPos({ buttonMax.x, buttonMax.y + ImGui::GetStyle().ItemSpacing.x }, ImGuiCond_Appearing, { 1.f, 0.f });
		if (ImGui::BeginPopup("ViewportSettingsPopup"))
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::TextUnformatted("Camera");
			ImGui::PopFont();
			ImGui::DragFloat3("Position", glm::value_ptr(m_AppRef.GetCamera().GetPosition()), 0.01f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(m_AppRef.GetCamera().GetRotation()), 0.01f);
			ImGui::Dummy({ 0.f, 10.f });
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::TextUnformatted("Gizmo");
			ImGui::PopFont();
			ImGui::SliderFloat("Snap Value", &m_GizmoSnap, 0.1f, 10.f, "%.1f");
			ImGuiUtils::HelpMarker("The Gizmo will snap to these values if LCTRL is held down.");
			const char* gizmoModes[] = { "Translate", "Rotate", "Scale" };
			ImGui::Combo("Mode", &m_GizmoMode, gizmoModes, IM_ARRAYSIZE(gizmoModes));
			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);

		Entity selectedEntity = m_AppRef.GetSelectedEntity();
		if (selectedEntity.IsValid())
		{
			TransformComponent& transformComp = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 entityTransform = transformComp.GetTransform();

			glm::mat4 cameraProj = m_AppRef.GetCamera().GetProjection();

			if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
				cameraProj[1][1] *= -1.0f;

			bool snap = Input::IsKeyPressed(Keyboard::LeftControl);
			float snapValues[3] = { m_GizmoSnap, m_GizmoSnap, m_GizmoSnap };
			ImGuizmo::OPERATION gizmoMode = m_GizmoMode == 0 ? ImGuizmo::TRANSLATE : m_GizmoMode == 1 ? ImGuizmo::ROTATE : ImGuizmo::SCALE;
			ImGuizmo::Manipulate(glm::value_ptr(m_AppRef.GetCamera().GetView()), glm::value_ptr(cameraProj), gizmoMode, ImGuizmo::LOCAL, glm::value_ptr(entityTransform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(entityTransform), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));
				transformComp.position = translation;
				transformComp.rotation = glm::radians(rotation);
				transformComp.scale = scale;
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
}