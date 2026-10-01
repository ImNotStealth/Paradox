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
		: Panel("Viewport"), m_AppRef((EditorApp&)Application::Get())
	{
		m_AppRef.GetCamera().Update(0.f);
	}

	void ViewportPanel::OnImGuiRender(bool* opened, float deltaTime)
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

		ImGuiUtils::Image(m_AppRef.GetCompositeFramebuffer()->GetAttachmentImage(0), viewportPanelSize, { 1, 1, 1, 1 }, uv0, uv1);
		DrawSettings();

		if (Input::IsMousePressed(Mouse::Button1))
			m_AppRef.GetCamera().UpdateInput(deltaTime);

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

		m_AppRef.GetCamera().Update(deltaTime);
	}

	void ViewportPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressEvent>(PX_BIND_EVENT_FN(ViewportPanel::OnInput));
	}

	void ViewportPanel::DrawSettings()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
		ImVec2 padding = ImGui::GetStyle().WindowPadding;
		const ImVec2 buttonSize = { 16.f, 16.f };
		const ImVec4 whiteColor = { 1.f, 1.f, 1.f, 1.f };
		const ImVec4 selectedColor = { 0.5f, 0.5f, 0.5f, 1.f };

		ImGui::SetNextWindowPos({ m_ViewportBounds[1].x - ImGui::GetStyle().ItemSpacing.x, m_ViewportBounds[0].y + ImGui::GetStyle().ItemSpacing.x }, ImGuiCond_Always, ImVec2(1.f, 0.f));
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4.f, 4.f });

		ImGui::Begin("ViewportButtons", nullptr, window_flags);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
		if (ImGuiUtils::IconButton("ViewportSettings", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_SETTINGS)), buttonSize))
			ImGui::OpenPopup("ViewportSettingsPopup");

		ImVec2 buttonMin = ImGui::GetItemRectMin();

		if (ImGuiUtils::IconButton("ViewportTranslate", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_TRANSLATE)), buttonSize, m_GizmoMode == 0 ? selectedColor : whiteColor))
			m_GizmoMode = 0;
		if (ImGuiUtils::IconButton("ViewportRotate", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_ROTATE)), buttonSize, m_GizmoMode == 1 ? selectedColor : whiteColor))
			m_GizmoMode = 1;
		if (ImGuiUtils::IconButton("ViewportScale", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_SCALE)), buttonSize, m_GizmoMode == 2 ? selectedColor : whiteColor))
			m_GizmoMode = 2;


		ImGui::DragFloat3("Position", glm::value_ptr(m_AppRef.GetCamera().GetPosition()), 0.01f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(m_AppRef.GetCamera().GetRotation()), 0.01f);

		ImGui::SetNextWindowPos({ buttonMin.x - ImGui::GetStyle().ItemSpacing.x, buttonMin.y - ImGui::GetStyle().WindowPadding.y }, ImGuiCond_Appearing, { 1.f, 0.f });
		ImGui::PopStyleVar(2); // Pop WindowPadding 4 and 0

		if (ImGui::BeginPopup("ViewportSettingsPopup"))
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1], ImGui::GetFontSize() * 1.2f);
			ImGui::TextUnformatted("Viewport Settings");
			ImGui::PopFont();
			ImGui::Separator();
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
			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
		
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	bool ViewportPanel::OnInput(KeyPressEvent& event)
	{
		switch (event.GetKeyCode())
		{
		case Keyboard::Z: m_GizmoMode = 0; return true;
		case Keyboard::X: m_GizmoMode = 1; return true;
		case Keyboard::C: m_GizmoMode = 2; return true;
		}
		return false;
	}
}