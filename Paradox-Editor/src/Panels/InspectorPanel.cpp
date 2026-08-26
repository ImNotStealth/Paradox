#include "pxpch.h"
#include "InspectorPanel.h"

#include "EditorApp.h"

#include <Paradox/ImGui/ImGuiUtils.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
	InspectorPanel::InspectorPanel()
		: Panel("Inspector"), m_AppRef(static_cast<class EditorApp&>(Application::Get()))
	{
		RegisterInspector<SpriteComponent>("Sprite", PX_BIND_EVENT_FN(OnSpriteComponent));

		m_MissingTexture = Texture2D::Create("Missing Icon", "Assets/Textures/Missing.png");
	}

	void InspectorPanel::OnImGuiRender(bool* opened)
	{
		PX_PROFILE_FUNCTION();

		ImGui::Begin("Inspector", opened);

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

		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
		ImGui::Text("ID: %u", (uint32_t)entity.GetComponent<IDComponent>().id);
		ImGui::PopStyleColor();

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
		ImGui::ColorEdit4("Tint", glm::value_ptr(comp.color));

		ImGuiUtils::Image(comp.texture ? comp.texture : m_MissingTexture, { 100.f, 100.f });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePathPayload"))
			{
				char* str = (char*)payload->Data;
				std::string path = std::filesystem::relative(str, Project::GetActive().GetProperties().assetPath).string();
				std::filesystem::path filePath = std::filesystem::path(Project::GetActive().GetProperties().assetPath / path);

				if (std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath) && !path.empty())
				{
					PX_WARN("Setting texture! {0}", filePath.string());
					TextureProperties properties;
					properties.minFilter = TextureFilter::Linear;
					properties.magFilter = TextureFilter::Nearest;
					properties.debugName = filePath.filename().string();
					Shared<Texture2D> texture = Texture2D::Create(properties, filePath.string());
					comp.texture = texture;
				}
				else
					PX_WARN("Failed to set Texture, payload returned non-existant path: {0}", path);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Button("Clear Texture"))
			comp.texture = nullptr;

		ImGui::DragFloat("Tiling Factor", &comp.tilingFactor, 0.1f, 0.f, 10.f, "%.2f");
		ImGui::DragFloat2("UV 0", glm::value_ptr(comp.uv0), 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::DragFloat2("UV 1", glm::value_ptr(comp.uv1), 0.1f, 0.0f, 0.0f, "%.2f");
	}
}