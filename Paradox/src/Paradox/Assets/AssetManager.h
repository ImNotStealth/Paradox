#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/AssetIndex.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

namespace Paradox
{
	//TODO: This is all temporary, ideally some functions should be split into Editor and Runtime.
	class PARADOX_API AssetManager
	{
	public:
		AssetManager(const std::filesystem::path& assetPath);
		~AssetManager();

		Shared<Asset> GetAsset(UUID id);

		Shared<AssetMetadata> GetMetadata(const std::filesystem::path& path);

		inline AssetIndex& GetAssetIndex() { return m_AssetIndex; }
		static AssetManager* Get() { PX_CORE_ASSERT(s_Instance); return s_Instance; }

		static inline void SetInstance(AssetManager* instance) { s_Instance = instance; }

		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

	private:
		template<typename T>
		void RegisterMetadataType(AssetType type);
		void CreateMissingMetaFiles();

	protected:
		AssetIndex m_AssetIndex;
		std::filesystem::path m_AssetPath;
		std::unordered_map<UUID, Shared<AssetMetadata>> m_Metadatas;
		std::unordered_map<AssetType, std::function<Unique<AssetMetadata>(std::filesystem::path)>> m_MetaFactories;

		static AssetManager* s_Instance;
		friend class AssetIndexPanel;
	};
}