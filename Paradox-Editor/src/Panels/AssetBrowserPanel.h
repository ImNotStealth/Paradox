#pragma once

#include "Panels/Panel.h"
#include "Events/EditorEvents.h"

#include <Paradox.h>
#include <Paradox/Events/InputEvents.h>
#include <Paradox/Assets/Metadata/AssetMetadata.h>

namespace Paradox
{
	class AssetBrowserPanel : public Panel
	{
	public:
		AssetBrowserPanel();

		void OnImGuiRender(bool* opened) override;
		void OnEvent(Event& event) override;

		bool OnProjectChanged(ProjectChangedEvent& event);
		bool OnMousePressed(MousePressEvent& event);

	private:
		struct AssetEntry
		{
			std::string name;
			std::filesystem::path path;
			bool isDirectory;
			Shared<AssetMetadata> metadata;
		};

		void DrawEntry(const AssetEntry& entry);
		void UpdateEntries();
		void UpdateFilteredIndices();

	private:
		std::filesystem::path m_AssetPath, m_CurrentPath, m_RelativePath;
		ImGuiTextFilter m_Filter;
		std::vector<AssetEntry> m_Entries;
		std::vector<size_t> m_FilteredIndices;
		bool m_UpdateRequested = false, m_FilteredIndicesDirty = false;
		bool m_WindowHovered = false;
		bool m_ShowMetaFiles = false;
		float m_CardSize = 100.f, m_Padding = 8.f;
		std::unordered_map<AssetType, Shared<Texture2D>> m_AssetIcons;
		std::unordered_map<std::filesystem::path, Shared<Texture2D>> m_ThumbnailCache;
	};
}