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

		for (const auto& uuid : m_QueuedForDeletion)
			DeleteAssetInstant(uuid);

		m_QueuedForInvalidation.clear();
		m_QueuedForDeletion.clear();
	}

	void AssetManager::InvalidateAsset(const UUID& id)
	{
		if (m_Assets.find(id) != m_Assets.end())
			m_QueuedForInvalidation.insert(id);
	}

	void AssetManager::DeleteAsset(const UUID& id)
	{
		InvalidateAsset(id);
		m_QueuedForDeletion.insert(id);
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

	void AssetManager::DeleteAssetInstant(const UUID& id)
	{
		const AssetIndex::IndexEntry& entry = m_AssetIndex.Get(id);
		const std::filesystem::path metaPath = m_AssetPath.parent_path() / entry.path;
		const std::filesystem::path resolvedPath = std::filesystem::path(metaPath).replace_extension();
		bool isDirectory = std::filesystem::is_directory(resolvedPath);

		PX_CORE_INFO(isDirectory ? "Deleting Asset folder (recursive): {0}" : "Deleting Asset: {0}", id.ToString());

		if (isDirectory)
		{
			const std::filesystem::path folderRel = std::filesystem::path(entry.path).replace_extension();

			std::vector<UUID> childAssets;
			for (auto& [uuid, entry] : m_AssetIndex)
			{
				if (uuid == id)
					continue;

				std::filesystem::path rel = entry.path.lexically_relative(folderRel);
				if (rel.empty() || *rel.begin() == std::filesystem::path("..") || *rel.begin() == std::filesystem::path("."))
					continue;

				childAssets.push_back(uuid);
			}

			for (const UUID& uuid : childAssets)
			{
				if (m_Metadatas.find(uuid) != m_Metadatas.end())
				{
					m_Metadatas[uuid].Reset();
					m_Metadatas.erase(uuid);
				}

				if (m_Assets.find(uuid) != m_Assets.end())
				{
					m_Assets[uuid].Reset();
					m_Assets.erase(uuid);
				}
			}
		}
		
		if (m_Metadatas.find(id) != m_Metadatas.end())
		{
			m_Metadatas[id].Reset();
			m_Metadatas.erase(id);
		}

		m_AssetIndex.Delete(id);
		m_AssetIndex.Serialize();

		if (std::filesystem::exists(metaPath))
			std::filesystem::remove(metaPath);

		if (std::filesystem::exists(resolvedPath))
			std::filesystem::remove_all(resolvedPath);
	}
}