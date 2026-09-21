#include "pxpch.h"
#include "AssetManager.h"

#include "Paradox/Assets/Metadata/Texture2DMetadata.h"
#include "Paradox/Assets/Metadata/FolderMetadata.h"

#define ASSET_INDEX_VERSION 1

namespace Paradox
{
	AssetManager* AssetManager::s_Instance = nullptr;

	AssetManager::AssetManager(const std::filesystem::path& assetPath)
		: m_AssetPath(assetPath), m_AssetIndex(assetPath)
	{
		PX_CORE_INFO("Created AssetManager at: {0}", assetPath.string());

		RegisterMetadataType<Texture2DMetadata>(AssetType::Texture2D);

		m_AssetIndex.Deserialize();
		CreateMissingMetaFiles();
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

	Shared<Asset> AssetManager::GetAsset(UUID id)
	{
		if (m_Assets.find(id) != m_Assets.end())
			return m_Assets[id];

		PX_CORE_ASSERT(m_AssetIndex.Contains(id), "Invalid UUID or not in Index.");
		PX_CORE_ASSERT(m_AssetIndex.Get(id).assetType != AssetType::Directory, "Directories cannot be retrieved as Assets.");
		
		Shared<AssetMetadata> metadata = GetMetadata(id);
		m_Assets[id] = metadata->CreateAsset();

		return m_Assets[id];
	}

	Shared<AssetMetadata> AssetManager::GetMetadata(UUID id)
	{
		PX_PROFILE_FUNCTION();

		if (!m_AssetIndex.Contains(id))
			return nullptr;

		const AssetIndex::IndexEntry& entry = m_AssetIndex.Get(id);

		if (m_Metadatas.find(id) != m_Metadatas.end())
			return m_Metadatas[id];

		Shared<AssetMetadata> metadata = nullptr;
		std::filesystem::path resolvedPath = m_AssetPath.parent_path() / std::filesystem::path(entry.path).replace_extension();
		if (std::filesystem::is_directory(resolvedPath))
			metadata = CreateShared<FolderMetadata>(resolvedPath);
		else
			metadata = m_MetaFactories[entry.assetType](resolvedPath);

		metadata->Deserialize();
		m_Metadatas[id] = metadata;
		return metadata;
	}

	Shared<AssetMetadata> AssetManager::GetMetadata(const std::filesystem::path& sourcePath)
	{
		PX_PROFILE_FUNCTION();

		if (!m_AssetIndex.Contains(sourcePath))
			return nullptr;

		return GetMetadata(m_AssetIndex.GetIdFromPath(sourcePath));
	}

	template<typename T>
	void AssetManager::RegisterMetadataType(AssetType type)
	{
		PX_CORE_ASSERT(m_MetaFactories.find(type) == m_MetaFactories.end(), "Duplicate AssetType.");
		m_MetaFactories[type] = [type](const std::filesystem::path& path) { return CreateUnique<T>(path); };
	}

	void AssetManager::CreateMissingMetaFiles()
	{
		// Index missing, create meta for all files
		for (auto& path : std::filesystem::recursive_directory_iterator(m_AssetPath))
		{
			if (path.is_directory())
			{
				Unique<FolderMetadata> folderMeta = CreateUnique<FolderMetadata>(path.path());
				std::filesystem::path metaPath = folderMeta->GetMetaPath();
				if (!std::filesystem::exists(metaPath))
				{
					PX_CORE_WARN("Meta missing for Folder, creating: {0}", folderMeta->GetMetaPath().string());
					folderMeta->Serialize();
					std::filesystem::path relPath = std::filesystem::relative(metaPath, m_AssetPath.parent_path());
					m_AssetIndex.Set(folderMeta->GetUUID(), { relPath, folderMeta->GetAssetType() });
				}
				continue;
			}

			AssetType assetType = Asset::GetTypeFromExtension(path.path().extension().string());
			if (assetType == AssetType::Unknown)
				continue;

			Unique<AssetMetadata> assetMeta = m_MetaFactories[assetType](path.path());
			std::filesystem::path metaPath = assetMeta->GetMetaPath();
			if (!std::filesystem::exists(metaPath))
			{
				PX_CORE_WARN("Meta missing for Asset, creating: {0}", metaPath.string());
				assetMeta->Serialize();
				std::filesystem::path relPath = std::filesystem::relative(metaPath, m_AssetPath.parent_path());
				m_AssetIndex.Set(assetMeta->GetUUID(), { relPath, assetMeta->GetAssetType() });
			}
		}
	}
}