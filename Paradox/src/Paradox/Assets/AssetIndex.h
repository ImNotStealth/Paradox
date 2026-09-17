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
			AssetType assetType = AssetType::Unknown;
		};

		AssetIndex(std::filesystem::path assetRootPath);

		inline bool Contains(UUID uuid) { return m_Index.find(uuid) != m_Index.end(); }
		inline bool Contains(std::filesystem::path path) { return m_PathToUUID.find(path) != m_PathToUUID.end(); }

		inline const IndexEntry& Get(UUID uuid) { PX_CORE_ASSERT(Contains(uuid), "Invalid handle or not in Index."); return m_Index[uuid]; }
		inline const IndexEntry& Get(std::filesystem::path path) { PX_CORE_ASSERT(Contains(path), "Invalid path or not in Index."); return m_Index[m_PathToUUID[path]]; }

		inline void Set(UUID uuid, const IndexEntry& entry) { PX_CORE_ASSERT(!Contains(uuid) && !Contains(entry.path)); m_Index[uuid] = entry; m_PathToUUID[entry.path] = uuid; }
		inline size_t Count() { return m_Index.size(); }

		void Serialize();
		void Deserialize();

		std::unordered_map<UUID, IndexEntry>::iterator begin() { return m_Index.begin(); }
		std::unordered_map<UUID, IndexEntry>::iterator end() { return m_Index.end(); }

	private:
		std::filesystem::path m_AssetRootPath, m_IndexFilePath;
		std::unordered_map<UUID, IndexEntry> m_Index;
		std::unordered_map<std::filesystem::path, UUID> m_PathToUUID;
	};
}