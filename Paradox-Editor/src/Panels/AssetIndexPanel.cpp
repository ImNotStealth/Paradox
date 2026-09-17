#include "pxpch.h"
#include "AssetIndexPanel.h"

#include <Paradox.h>
#include <imgui.h>

namespace Paradox
{
	AssetIndexPanel::AssetIndexPanel()
		: Panel("Asset Index") {}

	void AssetIndexPanel::OnImGuiRender(bool* opened)
	{
		PX_PROFILE_FUNCTION();

		ImGui::Begin("Asset Index", opened);

		ImGui::Text("Count: %d", AssetManager::Get()->GetAssetIndex().Count());

		ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("##AssetIndexPanel", 3, flags, ImGui::GetContentRegionAvail()))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("UUID");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("MetaPath");
			ImGui::TableHeadersRow();
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 15.f, 5.f });

			for (const auto& [uuid, entry] : AssetManager::Get()->GetAssetIndex())
			{
				ImGui::TableNextRow();

				std::string uuidStr = uuid.ToString();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(uuid.ToString().c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::TextUnformatted(Asset::AssetTypeToString(entry.assetType).c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::TextUnformatted(entry.path.string().c_str());
			}

			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		ImGui::End();
	}
}