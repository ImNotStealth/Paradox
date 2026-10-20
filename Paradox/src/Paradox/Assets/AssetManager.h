#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/AssetIndex.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

#include <set>

namespace Paradox
{
	//TODO: This is all temporary, ideally some functions should be split into Editor and Runtime.
	class PARADOX_API AssetManager
	{
	public:
		AssetManager(const std::filesystem::path& assetPath);
		~AssetManager();

		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

		Reference<Asset> GetAsset(const UUID& id);

		template<typename T>
		Reference<T> GetAsset(const UUID& id) { return GetAsset(id).AsA<T>(); }

		Reference<AssetMetadata> GetMetadata(const UUID& id);
		Reference<AssetMetadata> GetMetadata(const std::filesystem::path& sourcePath);

		template<typename T>
		Reference<T> GetMetadata(const UUID& id) { return GetMetadata(id).AsA<T>(); }

		bool Import(const std::filesystem::path& sourceFilePath, const std::filesystem::path& targetPath);
		void ScanPath(const std::filesystem::path& path, bool searchRecursive);

		void Update();
		
		void RenameAsset(const UUID& id, const std::string& newName);
		void InvalidateAsset(const UUID& id);
		void DeleteAsset(const UUID& id);

		inline AssetIndex& GetAssetIndex() { return m_AssetIndex; }
		static AssetManager* Get() { PX_CORE_ASSERT(s_Instance); return s_Instance; }
		static bool IsValid() { return s_Instance; }

		static inline void SetInstance(AssetManager* instance) { s_Instance = instance; }

	private:
		template<typename T>
		void RegisterMetadataType(AssetType type);
		void DeleteAssetInstant(const UUID& id);

	protected:
		AssetIndex m_AssetIndex;
		std::filesystem::path m_AssetPath;
		std::unordered_map<UUID, Reference<AssetMetadata>> m_Metadatas;
		std::unordered_map<AssetType, std::function<Reference<AssetMetadata>(std::filesystem::path)>> m_MetaFactories;
		std::unordered_map<UUID, Reference<Asset>> m_Assets;
		std::set<UUID> m_QueuedForInvalidation, m_QueuedForDeletion;

		static AssetManager* s_Instance;
		friend class AssetIndexPanel;
	};
}