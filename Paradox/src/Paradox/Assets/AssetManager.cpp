#include "pxpch.h"
#include "AssetManager.h"

#include "Paradox/Assets/Metadata/Texture2DMetadata.h"
#include "Paradox/Assets/Metadata/FolderMetadata.h"
#include "Paradox/Assets/Metadata/SceneMetadata.h"

#define ASSET_INDEX_VERSION 1

namespace Paradox
{
	AssetManager* AssetManager::s_Instance = nullptr;

	AssetManager::AssetManager(const std::filesystem::path& assetPath)
		: m_AssetPath(assetPath), m_AssetIndex(assetPath)
	{
		PX_CORE_INFO("Created AssetManager: {0}", assetPath.string());

		RegisterMetadataType<FolderMetadata>(AssetType::Directory);
		RegisterMetadataType<Texture2DMetadata>(AssetType::Texture2D);
		RegisterMetadataType<SceneMetadata>(AssetType::Scene);

		InitAssetIndex();
	}

	AssetManager::~AssetManager()
	{
		if (s_Instance == this)
			s_Instance = nullptr;

		PX_CORE_INFO("Destroyed AssetManager: {0}", m_AssetPath.string());
	}

	Reference<Asset> AssetManager::GetAsset(const UUID& id)
	{
		if (m_Assets.find(id) != m_Assets.end())
			return m_Assets[id];

		if (!m_AssetIndex.Contains(id))
			return nullptr;

		PX_CORE_ASSERT(m_AssetIndex.Get(id).assetType != AssetType::Directory, "Directories cannot be retrieved as Assets.");

		Reference<AssetMetadata> metadata = GetMetadata(id);
		m_Assets[id] = metadata->CreateAsset();

		return m_Assets[id];
	}

	Reference<AssetMetadata> AssetManager::GetMetadata(const UUID& id)
	{
		PX_PROFILE_FUNCTION();

		if (!m_AssetIndex.Contains(id))
			return nullptr;

		const AssetIndex::IndexEntry& entry = m_AssetIndex.Get(id);

		if (m_Metadatas.find(id) != m_Metadatas.end())
			return m_Metadatas[id];

		std::filesystem::path resolvedPath = m_AssetPath.parent_path() / std::filesystem::path(entry.path).replace_extension();
		Reference<AssetMetadata> metadata = m_MetaFactories[entry.assetType](resolvedPath);

		metadata->Deserialize();
		m_Metadatas[id] = metadata;
		return metadata;
	}

	Reference<AssetMetadata> AssetManager::GetMetadata(const std::filesystem::path& sourcePath)
	{
		PX_PROFILE_FUNCTION();

		if (!m_AssetIndex.Contains(sourcePath))
			return nullptr;

		return GetMetadata(m_AssetIndex.GetIdFromPath(sourcePath));
	}

	void AssetManager::Update()
	{
		for (auto& [uuid, asset] : m_Assets)
		{
			if (asset.GetRefCount() <= 1)
				m_QueuedForInvalidation.insert(uuid);
		}

		for (const auto& uuid : m_QueuedForInvalidation)
		{
			/*if (m_Assets[uuid].use_count() > 1)
				PX_CORE_WARN("Asset queued for deletion but references are still held: {0}", uuid.ToString());*/

			m_Assets[uuid].Reset();
			m_Assets.erase(uuid);
			PX_CORE_WARN("Invalidated {0}: {1}", Asset::AssetTypeToString(m_Metadatas[uuid]->GetAssetType()), uuid.ToString());
		}

		m_QueuedForInvalidation.clear();
	}

	void AssetManager::InvalidateAsset(const UUID& id)
	{
		if (m_Assets.find(id) != m_Assets.end())
			m_QueuedForInvalidation.insert(id);
	}

	template<typename T>
	void AssetManager::RegisterMetadataType(AssetType type)
	{
		PX_CORE_ASSERT(m_MetaFactories.find(type) == m_MetaFactories.end(), "Duplicate AssetType.");
		m_MetaFactories[type] = [type](const std::filesystem::path& path) { return CreateRef<T>(path); };
	}

	void AssetManager::InitAssetIndex()
	{
		m_AssetIndex.Deserialize();
	}
}