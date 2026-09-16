#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Asset.h"

namespace Paradox
{
	class AssetIndex
	{
	public:
		struct IndexEntry
		{
			std::filesystem::path path;
			AssetType assetType;
		};

		AssetIndex(std::filesystem::path assetRootPath);

		void Serialize();
		void Deserialize();

	private:
		std::filesystem::path m_AssetRootPath, m_IndexFilePath;
		std::unordered_map<UUID, IndexEntry> m_Index;
		std::unordered_map<std::filesystem::path, UUID> m_PathToUUID;
	};
}