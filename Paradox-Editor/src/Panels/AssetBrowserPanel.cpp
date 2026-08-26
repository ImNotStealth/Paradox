#include "pxpch.h"
#include "AssetBrowserPanel.h"

#include "Project/Project.h"

#include <Paradox/Core/FileSystem.h>
#include <Paradox/ImGui/ImGuiUtils.h>

namespace Paradox
{
	AssetBrowserPanel::AssetBrowserPanel()
		: Panel("Asset Browser")
	{
		m_AssetPath = Project::GetActive().GetProperties().assetPath;
		m_CurrentPath = m_AssetPath;

		m_FolderIcon = Texture2D::Create("Folder Icon", "Assets/Textures/folder.png");
	}

	void AssetBrowserPanel::OnImGuiRender(bool* opened)
	{
		PX_PROFILE_FUNCTION();
		ImGui::Begin("Assets", opened);

		m_WindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

		if (ImGui::Button("Refresh"))
			m_UpdateRequested = true;
		ImGui::SameLine();

		bool backEnabled = m_CurrentPath != m_AssetPath;
		if (!backEnabled)
			ImGui::BeginDisabled();
		if (ImGui::Button("Back"))
		{
			m_CurrentPath = m_CurrentPath.parent_path();
			m_UpdateRequested = true;
		}
		if (!backEnabled)
			ImGui::EndDisabled();
		ImGui::SameLine();

		if (std::filesystem::exists(m_CurrentPath))
		{
			std::filesystem::path relativePath = std::filesystem::relative(m_CurrentPath, Project::GetActive().GetProperties().path);
			ImGui::Text("%s", relativePath.string().c_str());
			ImGui::SameLine();
		}

		float filterWidth = 250.0f;
		float settingsButtonWidth = ImGui::CalcTextSize("Settings").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float targetX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - filterWidth - settingsButtonWidth - ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX(targetX);

		ImVec2 filterPos = ImGui::GetCursorScreenPos();
		static std::string previousFilter;
		m_Filter.Draw("##AssetBrowserPanelFilter", filterWidth);
		if (previousFilter != m_Filter.InputBuf)
		{
			previousFilter = m_Filter.InputBuf;
			m_FilteredIndicesDirty = true;
		}
		if (!m_Filter.IsActive())
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddText({ filterPos.x + 5.f, filterPos.y + 4.f }, ImGui::GetColorU32(ImGuiCol_TextDisabled), "Search...");
		}

		ImGui::SameLine();
		if (ImGui::Button("Settings"))
			ImGui::OpenPopup("SettingsPopup");

		if (ImGui::BeginPopup("SettingsPopup"))
		{
			ImGui::SliderFloat("Thumbnail Size", &m_ThumbnailSize, 100.f, 256.f);
			ImGui::EndPopup();
		}

		ImGui::BeginChild("AssetBrowserPanelChild", { 0, 0 }, 0, ImGuiWindowFlags_HorizontalScrollbar);

		int columnCount = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (m_ThumbnailSize + m_Padding)));
		ImGui::Columns(columnCount, 0, false);

		for (size_t index : m_FilteredIndices)
		{
			const AssetEntry& entry = m_Entries[index];
			DrawEntry(entry);
		}
		ImGui::EndChild();
		ImGui::End();

		if (m_UpdateRequested)
		{
			UpdateEntries();
			m_UpdateRequested = false;
		}

		if (m_FilteredIndicesDirty)
		{
			UpdateFilteredIndices();
			m_FilteredIndicesDirty = false;
		}
	}

	void AssetBrowserPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<ProjectChangedEvent>(PX_BIND_EVENT_FN(AssetBrowserPanel::OnProjectChanged));
		dispatcher.Dispatch<MousePressEvent>(PX_BIND_EVENT_FN(AssetBrowserPanel::OnMousePressed));
	}

	bool AssetBrowserPanel::OnProjectChanged(ProjectChangedEvent& event)
	{
		m_AssetPath = Project::GetActive().GetProperties().assetPath;
		m_CurrentPath = m_AssetPath;
		m_UpdateRequested = true;
		m_FilteredIndicesDirty = true;
		return false;
	}

	bool AssetBrowserPanel::OnMousePressed(MousePressEvent& event)
	{
		if (m_CurrentPath == m_AssetPath || !m_WindowHovered || event.GetKeyCode() != Mouse::Button3)
			return false;

		m_CurrentPath = m_CurrentPath.parent_path();
		m_UpdateRequested = true;
		return true;
	}

	void AssetBrowserPanel::DrawEntry(const AssetEntry& entry)
	{
		PX_PROFILE_FUNCTION();
		ImGui::PushID(entry.name.c_str());

		if (!entry.isDirectory && m_ThumbnailCache.find(entry.path) == m_ThumbnailCache.end())
			m_ThumbnailCache[entry.path] = Texture2D::Create(entry.name, entry.path.string());

		const float edgeOffset = 8.f;
		const float textLineHeight = ImGui::GetTextLineHeightWithSpacing() + edgeOffset * 2.0f;
		const float infoPanelHeight = textLineHeight;

		const ImVec2 topLeft = ImGui::GetCursorScreenPos();
		const ImVec2 infoTopLeft = { topLeft.x,				 topLeft.y + m_ThumbnailSize };
		const ImVec2 bottomRight = { topLeft.x + m_ThumbnailSize, topLeft.y + m_ThumbnailSize + infoPanelHeight };


		ImGui::BeginGroup();
		ImGui::InvisibleButton(entry.name.c_str(), { m_ThumbnailSize, m_ThumbnailSize + infoPanelHeight });
		bool hovered = ImGui::IsItemHovered();

		ImGui::SetCursorScreenPos(topLeft);
		auto* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(topLeft, bottomRight, hovered ? 0xFF3B3B3B : 0xFF2B2B2B, 6.0f);

		ImVec2 vecMin = topLeft, vecMax = { topLeft.x + m_ThumbnailSize, topLeft.y + m_ThumbnailSize };
		vecMin.x -= -edgeOffset;
		vecMin.y -= -edgeOffset;
		vecMax.x += -edgeOffset;
		vecMax.y += -edgeOffset;

		if (!entry.isDirectory)
		{	
			drawList->AddRectFilled(vecMin, vecMax, 0xFF202020);

			const uint32_t width = m_ThumbnailCache[entry.path]->GetWidth();
			const uint32_t height = m_ThumbnailCache[entry.path]->GetHeight();
			const float drawThumbSize = m_ThumbnailSize - edgeOffset * 2.f;

			ImVec2 sizeDiff = ImGuiUtils::FitSizeToSquare(width, height, drawThumbSize);
			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset + sizeDiff.x / 2.f, ImGui::GetCursorPosY() + edgeOffset + sizeDiff.y / 2.f });
			ImGuiUtils::Image(m_ThumbnailCache[entry.path], { drawThumbSize - sizeDiff.x, drawThumbSize - sizeDiff.y });

			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset, ImGui::GetCursorPosY() + sizeDiff.y / 2.f + 4.0f });
			ImGui::TextWrapped("%s", entry.name.c_str());
		}
		else
		{
			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset, ImGui::GetCursorPosY() + edgeOffset });
			ImGuiUtils::Image(m_FolderIcon, { m_ThumbnailSize - edgeOffset * 2.f, m_ThumbnailSize - edgeOffset * 2.f });
			if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentPath /= entry.path.filename();
				m_UpdateRequested = true;
			}
			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset, ImGui::GetCursorPosY() + 4.0f });
			ImGui::TextWrapped("%s", entry.name.c_str());
		}
		ImGui::EndGroup();

		if (ImGui::BeginPopupContextItem("AssetContextMenu"))
		{
			if (ImGui::MenuItem("Show on disk..."))
				FileSystem::ShowFolder(entry.isDirectory ? entry.path : entry.path.parent_path());

			if (!entry.isDirectory && ImGui::MenuItem("Open externally..."))
				FileSystem::OpenFileWithDefaultProgram(entry.path);
			
			ImGui::EndPopup();
		}

		if (!entry.isDirectory && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const uint32_t width = m_ThumbnailCache[entry.path]->GetWidth();
			const uint32_t height = m_ThumbnailCache[entry.path]->GetHeight();
			const float drawThumbSize = m_ThumbnailSize - edgeOffset * 2.f;

			ImVec2 sizeDiff = ImGuiUtils::FitSizeToSquare(width, height, drawThumbSize);
			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + sizeDiff.x / 2.f, ImGui::GetCursorPosY() + sizeDiff.y / 2.f });
			ImGuiUtils::Image(m_ThumbnailCache[entry.path], { drawThumbSize - sizeDiff.x, drawThumbSize - sizeDiff.y });

			std::string path = entry.path.string();
			ImGui::SetDragDropPayload("TexturePathPayload", path.c_str(), path.size() + 1, ImGuiCond_Once);
			ImGui::SetCursorPos({ ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + sizeDiff.y / 2.f + 4.0f });
			ImGui::TextWrapped("%s", entry.name.c_str());

			ImGui::EndDragDropSource();
		}

		ImGui::SetCursorScreenPos({ topLeft.x, bottomRight.y + m_Padding });
		ImGui::Dummy(ImVec2(0.0f, 0.0f));
		ImGui::NextColumn();
		ImGui::PopID();
	}

	void AssetBrowserPanel::UpdateEntries()
	{
		m_Entries.clear();
		if (!std::filesystem::exists(m_CurrentPath))
			return;

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			const auto& path = directoryEntry.path();
			m_Entries.push_back({ path.filename().string(), path, directoryEntry.is_directory() });
		}
		m_FilteredIndicesDirty = true;
	}

	void AssetBrowserPanel::UpdateFilteredIndices()
	{
		m_FilteredIndices.clear();
		for (size_t i = 0; i < m_Entries.size(); i++)
		{
			const AssetEntry& e = m_Entries[i];

			if (!m_Filter.PassFilter(e.name.c_str()))
				continue;

			m_FilteredIndices.push_back(i);
		}
	}
}