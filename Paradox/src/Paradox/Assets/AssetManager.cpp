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
		PX_CORE_INFO("Created AssetManager at: {0}", assetPath.string());

		RegisterMetadataType<FolderMetadata>(AssetType::Directory);
		RegisterMetadataType<Texture2DMetadata>(AssetType::Texture2D);
		RegisterMetadataType<SceneMetadata>(AssetType::Scene);

		ScanPath(m_AssetPath, true);
		m_AssetIndex.Serialize();
	}

	AssetManager::~AssetManager()
	{
		if (s_Instance == this)
			s_Instance = nullptr;

		for (auto& [uuid, meta] : m_Metadatas)
			meta->Serialize();

		m_AssetIndex.Serialize();
		PX_CORE_INFO("Destroyed AssetManager");
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

	bool AssetManager::Import(const std::filesystem::path& sourceFilePath, const std::filesystem::path& targetPath)
	{
		PX_CORE_INFO("Importing Asset: {0}", sourceFilePath.string());
		if (std::filesystem::exists(targetPath / sourceFilePath.filename()))
		{
			PX_CORE_ERROR("Failed to import: Asset already exists in target path");
			return false;
		}

		AssetType assetType = Asset::GetTypeFromExtension(sourceFilePath.extension().string());
		if (assetType == AssetType::Unknown)
		{
			PX_CORE_ERROR("Failed to import: Asset is of unknown type");
			return false;
		}

		if (m_MetaFactories.find(assetType) == m_MetaFactories.end())
		{
			PX_CORE_ERROR("Missing Metadata handler for AssetType: {0}, this is a bug!", Asset::AssetTypeToString(assetType));
			return false;
		}

		std::filesystem::path targetFilePath = targetPath / sourceFilePath.filename();
		Reference<AssetMetadata> assetMeta = m_MetaFactories[assetType](targetFilePath);
		std::filesystem::path metaPath = assetMeta->GetMetaPath();
		if (!std::filesystem::exists(metaPath))
		{
			PX_CORE_WARN("Meta missing for Asset, creating: {0}", metaPath.string());
			assetMeta->Serialize();
			std::filesystem::path relPath = std::filesystem::relative(metaPath, m_AssetPath.parent_path());
			m_AssetIndex.Set(assetMeta->GetUUID(), { relPath, assetMeta->GetAssetType() });
			m_AssetIndex.Serialize();
		}
		m_Metadatas[assetMeta->GetUUID()] = assetMeta;

		if (sourceFilePath != targetPath)
		{
			PX_CORE_INFO("Copying source asset from {0} to {1}", sourceFilePath.string(), targetPath.string());
			std::filesystem::copy_file(sourceFilePath, targetPath / sourceFilePath.filename());
		}

		return true;
	}

	void AssetManager::ScanPath(const std::filesystem::path& path, bool searchRecursive)
	{
		std::vector<std::filesystem::directory_entry> paths;
		if (searchRecursive)
			paths.assign(std::filesystem::recursive_directory_iterator(path), std::filesystem::recursive_directory_iterator());
		else
			paths.assign(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator());

		for (auto& path : std::filesystem::recursive_directory_iterator(path))
		{
			if (path.is_directory())
			{
				Unique<FolderMetadata> folderMeta = CreateUnique<FolderMetadata>(path.path());
				std::filesystem::path metaPath = folderMeta->GetMetaPath();
				if (!std::filesystem::exists(metaPath))
				{
					PX_CORE_WARN("Meta missing for Folder, creating: {0}", folderMeta->GetMetaPath().string());
					folderMeta->Serialize();
				}
				else
					folderMeta->Deserialize();

				if (!m_AssetIndex.Contains(folderMeta->GetUUID()))
				{
					std::filesystem::path relPath = std::filesystem::relative(metaPath, m_AssetPath.parent_path());
					m_AssetIndex.Set(folderMeta->GetUUID(), { relPath, folderMeta->GetAssetType() });
				}
				continue;
			}

			//TODO: Call Import()
			AssetType assetType = Asset::GetTypeFromExtension(path.path().extension().string());
			if (assetType == AssetType::Unknown)
				continue;

			if (m_MetaFactories.find(assetType) == m_MetaFactories.end())
			{
				PX_CORE_ERROR("Missing Metadata handler for AssetType: {0}, this is a bug!", Asset::AssetTypeToString(assetType));
				continue;
			}

			Reference<AssetMetadata> assetMeta = m_MetaFactories[assetType](path.path());
			std::filesystem::path metaPath = assetMeta->GetMetaPath();
			if (!std::filesystem::exists(metaPath))
			{
				PX_CORE_WARN("Meta missing for Asset, creating: {0}", metaPath.string());
				assetMeta->Serialize();
			}
			else
				assetMeta->Deserialize();

			if (!m_AssetIndex.Contains(assetMeta->GetUUID()))
			{
				std::filesystem::path relPath = std::filesystem::relative(metaPath, m_AssetPath.parent_path());
				m_AssetIndex.Set(assetMeta->GetUUID(), { relPath, assetMeta->GetAssetType() });
			}
		}
	}

	void AssetManager::Update()
	{
		for (auto& [uuid, asset] : m_Assets)
		{
			if (asset.GetRefCount() <= 1)
			{
				PX_CORE_WARN("Unloading unused {0}: {1}", Asset::AssetTypeToString(m_Metadatas[uuid]->GetAssetType()), uuid.ToString());
				m_QueuedForInvalidation.insert(uuid);
			}
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

	void AssetManager::DeleteAssetInstant(const UUID& id)
	{
		PX_CORE_INFO("Deleting Asset: {0}", id.ToString());

		if (m_Metadatas.find(id) != m_Metadatas.end())
		{
			//if (m_Metadatas[id].GetRefCount() > 1)
			//	PX_CORE_WARN("Metadata is still loaded in memory: {0}", id.ToString());

			m_Metadatas[id].Reset();
			m_Metadatas.erase(id);
		}

		const AssetIndex::IndexEntry& entry = m_AssetIndex.Get(id);
		const std::filesystem::path metaPath = m_AssetPath.parent_path() / entry.path;
		const std::filesystem::path resolvedPath = std::filesystem::path(metaPath).replace_extension();

		if (std::filesystem::exists(metaPath))
			std::filesystem::remove(metaPath);

		if (std::filesystem::exists(resolvedPath))
			std::filesystem::remove(resolvedPath);

		m_AssetIndex.Delete(id);
		m_AssetIndex.Serialize();
	}
}