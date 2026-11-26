#include "InspectorPanel.h"

#include "EditorApp.h"
#include "Utils/EditorIcons.h"

#include <Paradox/ImGui/ImGuiUtils.h>
#include <Paradox/Assets/Metadata/Texture2DMetadata.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
	InspectorPanel::InspectorPanel()
		: Panel("Inspector"), m_AppRef((EditorApp&)Application::Get())
	{
		RegisterInspector<SpriteComponent>("Sprite", PX_BIND_FN(OnSpriteComponent));
		RegisterInspector<CameraComponent>("Camera", PX_BIND_FN(OnCameraComponent));

		m_MissingTexture = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_MISSING));
	}

	void InspectorPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();

		if (!ImGui::Begin("Inspector", opened))
		{
			ImGui::End();
			return;
		}

		Entity entity = m_AppRef.GetSelectedEntity();
		if (!entity.IsValid())
		{
			ImGui::Text("No Entity selected");
			ImGui::End();
			return;
		}

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("Entity Name");
		ImGui::PopFont();

		float addButtonWidth = ImGui::CalcTextSize("Add Component...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float textInputWidth = ImGui::GetContentRegionAvail().x - addButtonWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::PushItemWidth(textInputWidth);
		ImGui::InputText("##Name", &entity.GetComponent<NameComponent>().name);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("Add Component..."))
			ImGui::OpenPopup("InspectorAddComponentPopup");

		ImGui::TextDisabled("ID: %s", entity.GetComponent<IDComponent>().id.ToString().c_str());

		ImGui::Dummy({ 0.f, 10.f });
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("Components");
		ImGui::PopFont();

		OnTransformComponent(entity);

		for (auto& [type, entry] : m_Inspectors)
		{
			entry.func(entity);
		}

		if (ImGui::BeginPopup("InspectorAddComponentPopup"))
		{
			for (auto& [type, entry] : m_Inspectors)
				entry.popupFunc(entity);

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	template<typename T>
	void InspectorPanel::RegisterInspector(const std::string& name, std::function<void(T&)> func)
	{
		InspectorEntry entry;
		entry.name = name;
		entry.popupFunc = [name](Entity& entity)
		{
			bool disabled = entity.HasComponents<T>();
			if (disabled)
				ImGui::BeginDisabled();

			if (ImGui::MenuItem(name.c_str()))
			{
				entity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}

			if (disabled)
				ImGui::EndDisabled();
		};
		entry.func = [this, name, func](Entity& entity)
		{
			DrawContainer<T>(name, entity, func);
		};

		m_Inspectors[typeid(T)] = entry;
	}

	template<typename T, typename Func>
	void InspectorPanel::DrawContainer(const std::string& name, Entity entity, Func func)
	{
		if (!entity.HasComponents<T>())
			return;

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_FramePadding;

		float buttonWidth = ImGui::CalcTextSize("...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float totalWidth = ImGui::GetContentRegionAvail().x;

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, totalWidth - buttonWidth);

		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 4.f);
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());

		ImGui::NextColumn();
		if (ImGui::Button("..."))
			ImGui::OpenPopup("ComponentSettingsPopup");
		ImGui::PopStyleVar();

		bool componentDeleted = false;
		if (ImGui::BeginPopup("ComponentSettingsPopup"))
		{
			bool deleteDisabled = typeid(T) == typeid(TransformComponent);
			if (deleteDisabled)
				ImGui::BeginDisabled();

			if (ImGui::MenuItem("Delete Component"))
				componentDeleted = true;

			if (deleteDisabled)
				ImGui::EndDisabled();

			ImGui::EndPopup();
		}

		ImGui::Columns(1);
		if (open)
		{
			func(entity.GetComponent<T>());
			ImGui::TreePop();
		}

		ImGui::Dummy({ 0.f, 5.f });

		if (componentDeleted)
			entity.RemoveComponents<T>();
	}

	// This is the only one that calls DrawContainer manually as it's not registered as a removable component.
	void InspectorPanel::OnTransformComponent(Entity& entity)
	{
		DrawContainer<TransformComponent>("Transform", entity, [](TransformComponent& comp)
		{
			ImGui::DragFloat3("Position", glm::value_ptr(comp.position), 0.1f, 0.0f, 0.0f, "%.2f");
			glm::vec3 rotation = glm::degrees(comp.rotation);
			ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f, 0.0f, 0.0f, "%.2f");
			comp.rotation = glm::radians(rotation);
			ImGui::DragFloat3("Scale", glm::value_ptr(comp.scale), 0.1f, 0.0f, 0.0f, "%.2f");
		});
	}

	void InspectorPanel::OnSpriteComponent(SpriteComponent& comp)
	{
		ImGuiUtils::TextureSlot(comp.texture, m_MissingTexture);

		// This could be an option where it forces the scale to match the image's ratio
		bool b = false;
		ImGui::Checkbox("Fixed Ratio", &b);
		ImGuiUtils::HelpMarker("Not functional right now.\nForces the entity's scale to match the texture's ratio (ie. a 256x128 texture will have a 2:1 scale).");
		ImGui::ColorEdit4("Tint", glm::value_ptr(comp.color));
		ImGui::DragFloat("Tiling Factor", &comp.tilingFactor, 0.1f, 0.f, 10.f, "%.2f");
		ImGui::DragFloat2("UV 0", glm::value_ptr(comp.uv0), 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::DragFloat2("UV 1", glm::value_ptr(comp.uv1), 0.1f, 0.0f, 0.0f, "%.2f");
	}

	void InspectorPanel::OnCameraComponent(CameraComponent &comp)
	{
		const char* projections[] = { "Perspective", "Orthographic" };
		CameraProjectionType projType = comp.camera.GetProjectionType();
		int projectionIndex = (int)projType;
		if (ImGui::Combo("Projection", &projectionIndex, projections, IM_ARRAYSIZE(projections)))
			comp.camera.SetProjectionType((CameraProjectionType)projectionIndex);

		ImGui::Checkbox("Use Viewport Size", &comp.useViewportSize);

		if (projType == CameraProjectionType::Perspective)
		{
			float fov = comp.camera.GetFieldOfView();
			if (ImGui::DragFloat("Field of View", &fov, 0.1f, 0.1f, 0.f, "%.1f"))
				comp.camera.SetFieldOfView(fov);
		}
		else
		{
			float size = comp.camera.GetOrthographicSize();
			if (ImGui::DragFloat("Orthographic Size", &size, 0.1f, 0.1f, 0.f, "%.1f"))
				comp.camera.SetOrthographicSize(size);
		}

		float nearClip = comp.camera.GetNearClip();
		if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f, 0.1f, 0.f, "%.1f"))
			comp.camera.SetNearClip(nearClip);

		float farClip = comp.camera.GetFarClip();
		if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, 0.1f, 0.f, "%.1f"))
			comp.camera.SetFarClip(farClip);
	}
}
