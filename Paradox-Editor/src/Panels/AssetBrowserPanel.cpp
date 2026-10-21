#include "AssetBrowserPanel.h"

#include "Project/Project.h"
#include "Panels/Popups/YesNoPanel.h"
#include "Utils/EditorIcons.h"
#include "EditorApp.h"

#include <Paradox/Core/FileSystem.h>
#include <Paradox/Assets/Metadata/FolderMetadata.h>
#include <Paradox/ImGui/ImGuiUtils.h>
#include <glm/gtc/type_ptr.hpp>
#include <misc/cpp/imgui_stdlib.h>

namespace Paradox
{
	AssetBrowserPanel::AssetBrowserPanel()
		: Panel("Asset Browser"), m_AppRef((EditorApp&)Application::Get())
	{
		m_AssetIcons[AssetType::Directory] = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_FOLDER));
		m_AssetIcons[AssetType::Unknown] = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_MISSING));
		m_AssetIcons[AssetType::Scene] = m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_SCENE));
	}

	void AssetBrowserPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();

		if (!ImGui::Begin("Assets", opened))
		{
			ImGui::End();
			return;
		}

		if (!AssetManager::IsValid())
		{
			ImGui::Text("Open a Project to start");
			ImGui::End();
			return;
		}

		m_WindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

		if (ImGuiUtils::IconButton("Import", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_IMPORT))))
		{
			std::filesystem::path path = FileSystem::SelectFile("Select an asset to import", "All files|*");
			if (!path.empty() && AssetManager::Get()->Import(path, m_CurrentPath))
				m_UpdateRequested = true;
		}
		ImGuiUtils::Tooltip("Import Asset...");
		ImGui::SameLine();

		if (ImGuiUtils::IconButton("Refresh", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_SYNC))))
			m_UpdateRequested = true;
		ImGuiUtils::Tooltip("Sync");
		ImGui::SameLine();

		bool backEnabled = m_CurrentPath != m_AssetPath;
		if (!backEnabled)
			ImGui::BeginDisabled();
		if (ImGuiUtils::IconButton("Back", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_BACK))))
		{
			m_CurrentPath = m_CurrentPath.parent_path();
			m_UpdateRequested = true;
		}
		ImGuiUtils::Tooltip("Back");
		if (!backEnabled)
			ImGui::EndDisabled();
		ImGui::SameLine();

		ImGui::Text("%s", m_RelativePath.string().c_str());
		ImGui::SameLine();

		float filterWidth = 250.0f;
		float settingsButtonWidth = 16.f + 12.f; // 6.f * 2.f (padding)
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
		if (ImGuiUtils::IconButton("Settings", m_AppRef.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_SETTINGS))))
			ImGui::OpenPopup("SettingsPopup");

		if (ImGui::BeginPopup("SettingsPopup"))
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1], ImGui::GetFontSize() * 1.2f);
			ImGui::TextUnformatted("Asset Browser Settings");
			ImGui::PopFont();
			ImGui::Separator();
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::TextUnformatted("Cards");
			ImGui::PopFont();
			ImGui::SliderFloat("Card Size", &m_CardSize, 100.f, 256.f);
			if (ImGui::MenuItem("Clear Texture Cache"))
				m_TextureCache.clear();
			ImGui::EndPopup();
		}

		ImGui::BeginChild("AssetBrowserPanelChild", { 0, 0 }, 0, ImGuiWindowFlags_HorizontalScrollbar);

		int columnCount = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (m_CardSize + m_Padding)));
		ImGui::Columns(columnCount, 0, false);

		for (size_t index : m_FilteredIndices)
			DrawEntry(m_Entries[index]);

		if (ImGui::BeginPopupContextWindow("AssetContextMenu", ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Scene"))
			{
				Scene newScene = Scene("New Scene");
				newScene.Serialize(m_CurrentPath / (newScene.GetName() + ".pscn"));
				m_UpdateRequested = true;
			}
			if (ImGui::MenuItem("Create Folder"))
			{
				std::filesystem::create_directories(m_CurrentPath / "New Folder");
				m_UpdateRequested = true;
			}

			ImGui::EndPopup();
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
		m_AssetPath = Project::GetActive()->GetProperties().assetPath;
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

	void AssetBrowserPanel::DrawEntry(AssetEntry& entry)
	{
		PX_PROFILE_FUNCTION();
		if (!entry.metadata.Get())
		{
			PX_WARN("Entry at {0} has no metadata, requesting refresh", entry.path.string());
			m_UpdateRequested = true;
			return;
		}

		ImGui::PushID(entry.metadata->GetUUID().ToString().c_str());

		AssetType assetType = entry.isDirectory ? AssetType::Directory : Asset::GetTypeFromExtension(entry.path.extension().string());
		bool isTexture = assetType == AssetType::Texture2D;

		if (isTexture && (m_TextureCache.find(entry.metadata->GetUUID()) == m_TextureCache.end() || !m_TextureCache[entry.metadata->GetUUID()]))
			m_TextureCache[entry.metadata->GetUUID()] = AssetManager::Get()->GetAsset<Texture2D>(entry.metadata->GetUUID());

		const float edgeOffset = 6.f;
		const float infoPanelHeight = (ImGui::GetTextLineHeightWithSpacing() + edgeOffset) * 2.0f;

		const ImVec2 topLeft = ImGui::GetCursorScreenPos();
		const ImVec2 infoTopLeft = { topLeft.x,				 topLeft.y + m_CardSize };
		const ImVec2 bottomRight = { topLeft.x + m_CardSize, topLeft.y + m_CardSize + infoPanelHeight };
		const float drawThumbSize = m_CardSize - edgeOffset * 2.f;

		ImGui::BeginGroup();
		ImGui::InvisibleButton(entry.name.c_str(), { m_CardSize, m_CardSize + infoPanelHeight });
		bool hovered = ImGui::IsItemHovered();

		ImGui::SetCursorScreenPos(topLeft);
		auto* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(topLeft, bottomRight, hovered ? 0xFF3B3B3B : 0xFF2B2B2B, 6.0f);

		ImVec2 vecMin = topLeft, vecMax = { topLeft.x + m_CardSize, topLeft.y + m_CardSize };
		vecMin.x -= -edgeOffset;
		vecMin.y -= -edgeOffset;
		vecMax.x += -edgeOffset;
		vecMax.y += -edgeOffset;

		if (!entry.isDirectory)
		{
			const Reference<Texture2D> icon = isTexture ? m_TextureCache[entry.metadata->GetUUID()] : m_AssetIcons[assetType];
			if (isTexture)
			{
				drawList->AddRectFilled(vecMin, vecMax, 0xFF202020);
				const uint32_t width = icon->GetWidth();
				const uint32_t height = icon->GetHeight();
				ImVec2 sizeDiff = ImGuiUtils::FitSizeToSquare(width, height, drawThumbSize);
				ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset + sizeDiff.x / 2.f, ImGui::GetCursorPosY() + edgeOffset + sizeDiff.y / 2.f });
				ImGuiUtils::Image(icon, { drawThumbSize - sizeDiff.x, drawThumbSize - sizeDiff.y });
			}
			else
			{
				ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset, ImGui::GetCursorPosY() + edgeOffset });
				ImGuiUtils::Image(icon, { drawThumbSize, drawThumbSize });
			}

			if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.metadata->GetAssetType() == AssetType::Scene)
					m_AppRef.SetScene(AssetManager::Get()->GetAsset<Scene>(entry.metadata->GetUUID()));
				else
					m_AppRef.GetPanelManager().OpenAssetEditor(entry.metadata);
			}
		}
		else
		{
			Reference<FolderMetadata> folderMetadata = entry.metadata.AsA<FolderMetadata>();
			glm::vec3 folderColor = folderMetadata->GetColor();

			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + edgeOffset, ImGui::GetCursorPosY() + edgeOffset });
			ImGuiUtils::Image(m_AssetIcons[AssetType::Directory], { drawThumbSize, drawThumbSize }, ImVec4(folderColor.r, folderColor.g, folderColor.b, 1.f));
			if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentPath /= entry.path.filename();
				m_UpdateRequested = true;
			}
		}

		std::string label = entry.name;
		const float maxHeight = ImGui::GetTextLineHeight() * 3.f;
		if (ImGui::CalcTextSize(label.c_str(), nullptr, false, drawThumbSize).y >= maxHeight)
		{
			while (!label.empty() &&
				ImGui::CalcTextSize((label + "...").c_str(), nullptr, false, drawThumbSize).y >= maxHeight)
				label.pop_back();
			label += "...";
		}

		// File name Text / Rename field
		ImGui::SetCursorScreenPos({ vecMin.x, vecMax.y + edgeOffset });
		if (!entry.isRenaming)
		{
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + drawThumbSize);
			ImGui::TextUnformatted(label.c_str());
			ImGui::PopTextWrapPos();
		}
		else
		{
			if (entry.focusRename)
			{
				ImGui::SetKeyboardFocusHere(0);
				entry.focusRename = false;
			}
			ImGui::PushItemWidth(drawThumbSize);
			ImGui::InputText("##", &m_RenameBuffer);
			ImGui::PopItemWidth();

			if (ImGui::IsItemDeactivated())
			{
				if (!m_RenameBuffer.empty())
				{
					AssetManager::Get()->RenameAsset(entry.metadata->GetUUID(), m_RenameBuffer);
					m_UpdateRequested = true;
				}
				m_RenameBuffer.clear();
				entry.isRenaming = false;
			}
		}

		// AssetType Text
		ImGui::PushFont(nullptr, ImGui::GetFontSize() * 0.8f);
		std::string assetTypeStr = Asset::AssetTypeToString(assetType);
		ImVec2 textSize = ImGui::CalcTextSize(assetTypeStr.c_str());
		ImGui::SetCursorScreenPos({ bottomRight.x - textSize.x - edgeOffset, bottomRight.y - textSize.y - edgeOffset });
		ImGui::TextDisabled("%s", assetTypeStr.c_str());
		ImGui::PopFont();

		ImGui::EndGroup();

		if (label != entry.name && ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(entry.name.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		if (ImGui::BeginPopupContextItem("AssetEntryContextMenu"))
		{
			if (ImGui::MenuItem("Show on disk..."))
				FileSystem::ShowFolder(entry.isDirectory ? entry.path : entry.path.parent_path());


			if (entry.isDirectory)
			{
				if (ImGui::Selectable("Change Color...", false, ImGuiSelectableFlags_NoAutoClosePopups))
					ImGui::OpenPopup("ChangeColorPopup");

				if (ImGui::BeginPopup("ChangeColorPopup"))
				{
					Reference<FolderMetadata> folderMetadata = entry.metadata.AsA<FolderMetadata>();
					ImGui::ColorPicker3("##FolderColor", glm::value_ptr(folderMetadata->GetColor()), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
					ImGui::EndPopup();
				}
			}
			else if (ImGui::MenuItem("Open externally..."))
				FileSystem::OpenFileWithDefaultProgram(entry.path);

			ImGui::Separator();
			if (ImGui::MenuItem("Rename"))
			{
				entry.isRenaming = true;
				entry.focusRename = true;
				m_RenameBuffer = entry.name;
			}
			if (ImGui::MenuItem("Delete"))
			{
				const UUID& assetId = entry.metadata->GetUUID();
				const std::string title = entry.isDirectory ? "Delete Folder?" : "Delete Asset?";
				const std::string msg = entry.isDirectory ? "This will erase the folder, including all the assets inside from disk!" : "This will erase the asset from disk!";
				m_AppRef.GetPanelManager().OpenPopup<YesNoPanel>(title, msg,
				[assetId](bool value)
				{
					if (value)
						AssetManager::Get()->DeleteAsset(assetId);
				});
			}

			ImGui::EndPopup();
		}

		if (assetType == AssetType::Texture2D && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const Reference<Texture2D> texture = m_TextureCache[entry.metadata->GetUUID()];
			const uint32_t width = texture->GetWidth();
			const uint32_t height = texture->GetHeight();

			ImVec2 sizeDiff = ImGuiUtils::FitSizeToSquare(width, height, drawThumbSize);
			ImGui::SetCursorPos({ ImGui::GetCursorPosX() + sizeDiff.x / 2.f, ImGui::GetCursorPosY() + sizeDiff.y / 2.f });
			ImGuiUtils::Image(texture, { drawThumbSize - sizeDiff.x, drawThumbSize - sizeDiff.y });

			std::string path = entry.path.string();
			ImGui::SetDragDropPayload("TextureUUIDPayload", entry.metadata->GetUUID().ToString().c_str(), entry.metadata->GetUUID().ToString().size() + 1, ImGuiCond_Once);
			ImGui::SetCursorPos({ ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + sizeDiff.y / 2.f + 4.0f });
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + drawThumbSize);
			ImGui::TextUnformatted(entry.name.c_str());
			ImGui::PopTextWrapPos();

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
		m_TextureCache.clear();
		if (!std::filesystem::exists(m_CurrentPath))
			return;

		AssetManager::Get()->ScanPath(m_CurrentPath, false);

		m_RelativePath = std::filesystem::relative(m_CurrentPath, Project::GetActive()->GetProperties().path);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			const std::filesystem::path& path = directoryEntry.path();

			if (path.extension() == ".pm")
				continue;

			bool isDirectory = directoryEntry.is_directory();
			AssetType assetType = isDirectory ? AssetType::Directory : Asset::GetTypeFromExtension(path.extension().string());
			std::filesystem::path relativePath = std::filesystem::relative(path, Project::GetActive()->GetProperties().path);
			m_Entries.push_back({ isDirectory ? path.filename().string() : path.stem().string(), path, isDirectory, false, false, assetType != AssetType::Unknown ? AssetManager::Get()->GetMetadata(relativePath) : nullptr});
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