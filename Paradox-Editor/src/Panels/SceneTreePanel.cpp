#include "pxpch.h"
#include "SceneTreePanel.h"

#include "EditorApp.h"

#include <imgui.h>

namespace Paradox
{
	SceneTreePanel::SceneTreePanel()
		: Panel("Scene Tree"), m_AppRef(static_cast<class EditorApp&>(Application::Get())) {}

	void SceneTreePanel::OnImGuiRender()
	{
		PX_PROFILE_FUNCTION();

		ImGui::Begin("Scene Tree");

		m_WindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

		m_Scene = m_AppRef.GetScene();
		if (!m_Scene)
		{
			ImGui::End();
			return;
		}

		for (auto& entity : m_Scene->GetEntitiesWith<IDComponent>())
		{
			DrawEntry(entity);
		}

		if (ImGui::BeginPopupContextWindow("SceneTreeContextMenu", ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Scene->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneTreePanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher = EventDispatcher(event);
		dispatcher.Dispatch<KeyPressEvent>(PX_BIND_EVENT_FN(SceneTreePanel::OnKeyPress));
	}

	void SceneTreePanel::DrawEntry(Entity& entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetComponent<NameComponent>().name.c_str());

		if (ImGui::IsItemClicked())
		{
			PX_WARN("Entity selected: {0}", entity.GetComponent<NameComponent>().name);
			m_AppRef.SetSelectedEntity(entity);
		}

		bool entityDestroyed = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete", "Delete"))
				entityDestroyed = true;

			ImGui::EndPopup();
		}
		
		if (opened)
			ImGui::TreePop();

		if (entityDestroyed)
			m_Scene->DestroyEntity(entity);
	}

	bool SceneTreePanel::OnKeyPress(KeyPressEvent& event)
	{
		// Destroy selected entity
		if (event.GetKeyCode() == Keyboard::Delete && m_WindowHovered && m_AppRef.GetSelectedEntity().IsValid())
		{
			m_Scene->DestroyEntity(m_AppRef.GetSelectedEntity());
			m_AppRef.SetSelectedEntity(Entity());
			return true;
		}

		return false;
	}
}