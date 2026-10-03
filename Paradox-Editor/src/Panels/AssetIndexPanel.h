#pragma once

#include "Panels/Panel.h"

#include <Paradox/Scene/UUID.h>

namespace Paradox
{
	class AssetIndexPanel : public Panel
	{
	public:
		AssetIndexPanel();

		void OnImGuiRender(bool* opened, float deltaTime) override;

	private:
		struct RowEntry
		{
			UUID uuid;
			std::string uuidStr;
			std::string assetType;
			std::string path;
			size_t refCount = 0;
		};

		std::vector<RowEntry> m_Entries;
		size_t m_LastCount = 0;
		bool m_RebuildEntries = false, m_ResortEntries = false;
	};
}