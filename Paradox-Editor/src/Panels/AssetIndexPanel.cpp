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

		for (const auto& [uuid, entry] : AssetManager::Get()->GetAssetIndex())
		{
			ImGui::Text("%s: %s / %s", uuid.ToString().c_str(), Asset::AssetTypeToString(entry.assetType).c_str(), entry.path.string().c_str());
		}

		ImGui::End();
	}
}