#pragma once

#include "Panels/Panel.h"
#include "Events/EditorEvents.h"

namespace Paradox
{
	class AssetBrowserPanel : public Panel
	{
	public:
		AssetBrowserPanel();

		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		bool OnProjectChanged(ProjectChangedEvent& event);

	private:
		void UpdateEntries();

	private:
		struct AssetEntry
		{
			std::string name;
			std::filesystem::path path;
			bool isDirectory;
		};

		std::filesystem::path m_AssetPath, m_CurrentPath;
		ImGuiTextFilter m_Filter;
		std::vector<AssetEntry> m_Entries;
		bool m_UpdateRequested = false;
		float m_ThumbnailSize = 80.f, m_Padding = 8.f;
	};
}