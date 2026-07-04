#include "AssetBrowserPanel.h"

#include "Project/Project.h"

#include <Paradox/ImGui/ImGuiUtils.h>

namespace Paradox
{
	AssetBrowserPanel::AssetBrowserPanel()
		: Panel("Asset Browser")
	{
		m_AssetPath = Project::GetActive().GetProperties().assetPath;
		m_CurrentPath = m_AssetPath;

		m_FolderIcon = Texture2D::Create("Folder Icon", "textures/Controls.png");
		m_FileIcon = Texture2D::Create("File Icon", "textures/texture.jpg");
	}

	void AssetBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets");

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
			ImGui::SliderFloat("Thumbnail Size", &m_ThumbnailSize, 16.f, 256.f);
			ImGui::SliderFloat("Padding", &m_Padding, 0.f, 32.f);
			ImGui::EndPopup();
		}

		ImGui::BeginChild("AssetBrowserPanelChild", { 0, 0 }, 0, ImGuiWindowFlags_HorizontalScrollbar);

		int columnCount = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (m_ThumbnailSize + m_Padding)));
		ImGui::Columns(columnCount, 0, false);

		for (size_t index : m_FilteredIndices)
		{
			const AssetEntry& entry = m_Entries[index];
			ImGui::PushID(entry.name.c_str());
			ImGuiUtils::Image(entry.isDirectory ? m_FolderIcon : m_FileIcon, { m_ThumbnailSize, m_ThumbnailSize });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && entry.isDirectory)
			{
				m_CurrentPath /= entry.path.filename();
				m_UpdateRequested = true;
			}

			ImGui::TextWrapped("%s", entry.name.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
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
	}

	bool AssetBrowserPanel::OnProjectChanged(ProjectChangedEvent& event)
	{
		m_AssetPath = Project::GetActive().GetProperties().assetPath;
		m_CurrentPath = m_AssetPath;
		m_UpdateRequested = true;
		m_FilteredIndicesDirty = true;
		return false;
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