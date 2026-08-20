#include "pxpch.h"
#include "InspectorPanel.h"

#include "EditorApp.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
	InspectorPanel::InspectorPanel()
		: Panel("Inspector"), m_AppRef(static_cast<class EditorApp&>(Application::Get()))
	{
		RegisterInspector<SpriteComponent>("Sprite", PX_BIND_EVENT_FN(OnSpriteComponent));
	}

	void InspectorPanel::OnImGuiRender()
	{
		PX_PROFILE_FUNCTION();

		ImGui::Begin("Inspector");

		Entity& entity = m_AppRef.GetSelectedEntity();
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
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowOverlap |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = 14.f;

		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
		ImGui::PopStyleVar();

		if (open)
		{
			func(entity.GetComponent<T>());
			ImGui::TreePop();
		}
	}

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
	}
}