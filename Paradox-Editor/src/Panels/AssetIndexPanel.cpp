#include "AssetIndexPanel.h"

#include <Paradox.h>
#include <imgui.h>

namespace Paradox
{
	enum AssetIndexColumn : ImGuiID
	{
		Column_UUID = 0,
		Column_Type,
		Column_MetaPath,
		Column_AssetRefCount,
		Column_MetaRefCount
	};

	AssetIndexPanel::AssetIndexPanel()
		: Panel("Asset Index") {}

	void AssetIndexPanel::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();

		if (!ImGui::Begin("Asset Index", opened))
		{
			ImGui::End();
			return;
		}

		if (!AssetManager::IsValid())
		{
			ImGui::Text("No active AssetManager instance found.");
			ImGui::End();
			return;
		}

		if (ImGui::Button("Refresh"))
			m_RebuildEntries = true;
		
		ImGui::SameLine();

		const size_t indexCount = AssetManager::Get()->GetAssetIndex().Count();
		ImGui::Text("Count: %ld", indexCount);

		if (indexCount != m_LastCount)
			m_RebuildEntries = true;

		std::unordered_map<UUID, Reference<Asset>>& loadedAssets = AssetManager::Get()->m_Assets;
		std::unordered_map<UUID, Reference<AssetMetadata>>& loadedMetadatas = AssetManager::Get()->m_Metadatas;
		if (m_RebuildEntries)
		{
			m_Entries.clear();
			m_Entries.reserve(indexCount);

			for (const auto& [uuid, entry] : AssetManager::Get()->GetAssetIndex())
			{
				RowEntry& rowEntry = m_Entries.emplace_back();
				rowEntry.uuid = uuid;
				rowEntry.uuidStr = uuid.ToString();
				rowEntry.assetType = Asset::AssetTypeToString(entry.assetType);
				rowEntry.path = entry.path.string();

				auto itAsset = loadedAssets.find(uuid);
				rowEntry.assetRefCount = itAsset != loadedAssets.end() ? itAsset->second.GetRefCount() : 0;

				auto itMeta = loadedMetadatas.find(uuid);
				rowEntry.metaRefCount = itMeta != loadedMetadatas.end() ? itMeta->second.GetRefCount() : 0;
			}

			m_LastCount = indexCount;
			m_RebuildEntries = false;
			m_ResortEntries = true;
		}

		ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("##AssetIndexPanel", 5, flags, ImGui::GetContentRegionAvail()))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("UUID", ImGuiTableColumnFlags_DefaultSort, 0.f, Column_UUID);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None, 0.f, Column_Type);
			ImGui::TableSetupColumn("MetaPath", ImGuiTableColumnFlags_None, 0.f, Column_MetaPath);
			ImGui::TableSetupColumn("Asset RefCount", ImGuiTableColumnFlags_None, 0.f, Column_AssetRefCount);
			ImGui::TableSetupColumn("Meta RefCount", ImGuiTableColumnFlags_None, 0.f, Column_MetaRefCount);
			ImGui::TableHeadersRow();
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 15.f, 5.f });

			if (ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs())
			{
				if (sortSpecs->SpecsDirty || m_RebuildEntries)
				{
					if (sortSpecs->SpecsCount > 0)
					{
						const ImGuiTableColumnSortSpecs& spec = sortSpecs->Specs[0];
						const bool ascending = spec.SortDirection == ImGuiSortDirection_Ascending;

						std::sort(m_Entries.begin(), m_Entries.end(), [&](const RowEntry& a, const RowEntry& b)
						{
							int16_t delta = 0;
							switch (spec.ColumnUserID)
							{
							case Column_UUID: delta = a.uuidStr.compare(b.uuidStr); break;
							case Column_Type: delta = a.assetType.compare(b.assetType); break;
							case Column_MetaPath: delta = a.path.compare(b.path); break;
							case Column_AssetRefCount:
								delta = a.assetRefCount < b.assetRefCount ? -1 : a.assetRefCount > b.assetRefCount ? 1 : 0;
								break;
							case Column_MetaRefCount:
								delta = a.metaRefCount < b.metaRefCount ? -1 : a.metaRefCount > b.metaRefCount ? 1 : 0;
								break;
							}

							if (delta != 0)
								return ascending ? (delta < 0) : (delta > 0);

							return a.uuidStr < b.uuidStr;
						});
					}

					sortSpecs->SpecsDirty = false;
					m_ResortEntries = false;
				}
			}

			ImGuiListClipper clipper;
			clipper.Begin((int)m_Entries.size());
			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					const RowEntry& entry = m_Entries[i];

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted(entry.uuidStr.c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::TextUnformatted(entry.assetType.c_str());

					ImGui::TableSetColumnIndex(2);
					ImGui::TextUnformatted(entry.path.c_str());

					ImGui::TableSetColumnIndex(3);
					const auto itAsset = loadedAssets.find(entry.uuid);
					ImGui::TextUnformatted(itAsset == loadedAssets.end() ? "Not loaded" : std::to_string(itAsset->second.GetRefCount()).c_str());
				
					ImGui::TableSetColumnIndex(4);
					const auto itMeta = loadedMetadatas.find(entry.uuid);
					ImGui::TextUnformatted(itMeta == loadedMetadatas.end() ? "Not loaded" : std::to_string(itMeta->second.GetRefCount()).c_str());
				}
			}

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		ImGui::End();

		m_LastCount = indexCount;
	}
}