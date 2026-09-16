#include "pxpch.h"
#include "AssetManager.h"

#include "Paradox/Assets/Metadata/Texture2DMetadata.h"
#include "Paradox/Assets/Metadata/FolderMetadata.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>

#define ASSET_INDEX_VERSION 1

namespace Paradox
{
	AssetManager* AssetManager::s_Instance = nullptr;

	AssetManager::AssetManager(std::filesystem::path assetPath)
		: m_AssetPath(assetPath), m_AssetIndex(assetPath)
	{
		PX_CORE_INFO("Created AssetManager at: {0}", assetPath.string());

		RegisterMetadata<Texture2DMetadata>(AssetType::Texture2D, {".png", ".jpg", ".jpeg"});

		m_AssetIndex.Deserialize();
		UpdateMetadata();
		m_AssetIndex.Serialize();

		//Maybe don't set this when creating the AssetManager for engine assets?
		s_Instance = this;
	}

	Shared<Asset> AssetManager::GetAsset(UUID id)
	{
		PX_CORE_ASSERT(m_Index.find(id) != m_Index.end(), "Invalid handle or not in Index.");
		return nullptr;
	}

	template<typename T>
	void AssetManager::RegisterMetadata(AssetType type, std::vector<std::string> fileExtensions)
	{
		PX_CORE_ASSERT(m_MetaFactories.find(type) == m_MetaFactories.end(), "Duplicate AssetType.");
		m_MetaFactories[type] = [type](std::filesystem::path path) { return CreateUnique<T>(path); };

		for (const std::string& extension : fileExtensions)
		{
			PX_CORE_ASSERT(m_AssetExtensions.find(extension) == m_AssetExtensions.end(), "Duplicate file extension.");
			m_AssetExtensions[extension] = type;
		}
	}

	void AssetManager::UpdateMetadata()
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
					m_Index[folderMeta->GetUUID()] = { metaPath, folderMeta->GetAssetType() };
				}
				continue;
			}

			AssetType assetType = GetTypeFromExtension(path.path().extension().string());
			if (assetType == AssetType::Unknown)
				continue;

			Unique<AssetMetadata> assetMeta = m_MetaFactories[assetType](path.path());
			std::filesystem::path metaPath = assetMeta->GetMetaPath();
			if (!std::filesystem::exists(metaPath))
			{
				PX_CORE_WARN("Meta missing for Asset, creating: {0}", metaPath.string());
				assetMeta->Serialize();
				m_Index[assetMeta->GetUUID()] = { metaPath, assetMeta->GetAssetType() };
			}
		}
	}

	AssetType AssetManager::GetTypeFromExtension(const std::string& extension)
	{
		if (m_AssetExtensions.find(extension) == m_AssetExtensions.end())
			return AssetType::Unknown;

		return m_AssetExtensions[extension];
	}
}