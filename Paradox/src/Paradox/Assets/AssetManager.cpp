#include "pxpch.h"
#include "AssetManager.h"

#include "Paradox/Assets/Metadata/Texture2DMetadata.h"
#include "Paradox/Assets/Metadata/FolderMetadata.h"

namespace Paradox
{
	AssetManager* AssetManager::s_Instance = nullptr;

	AssetManager::AssetManager(std::filesystem::path assetPath)
		: m_AssetPath(assetPath)
	{
		PX_CORE_INFO("Created AssetManager at: {0}", assetPath.string());

		RegisterHandler<Texture2DMetadata>(AssetType::Texture2D, {".png", ".jpg", ".jpeg"});

		LoadIndex();
		UpdateMetadata();

		//Maybe don't set this when creating the AssetManager for engine assets?
		s_Instance = this;
	}

	Shared<Asset> AssetManager::GetAsset(UUID id)
	{
		PX_CORE_ASSERT(m_Index.find(id) != m_Index.end(), "Invalid handle or not in Index.");
		return nullptr;
	}

	template<typename T>
	void AssetManager::RegisterHandler(AssetType type, std::vector<std::string> fileExtensions)
	{
		PX_CORE_ASSERT(m_MetaFactories.find(type) == m_MetaFactories.end(), "Duplicate AssetType.");
		m_MetaFactories[type] = [type](std::filesystem::path path) { return CreateUnique<T>(path); };

		for (const std::string& extension : fileExtensions)
		{
			PX_CORE_ASSERT(m_AssetExtensions.find(extension) == m_AssetExtensions.end(), "Duplicate file extension.");
			m_AssetExtensions[extension] = type;
		}
	}

	void AssetManager::LoadIndex()
	{
		if (!std::filesystem::exists(m_AssetPath / ".." / "Index.pi"))
		{
			PX_CORE_ERROR("AssetManager: Index file missing");
			return;
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
				if (!std::filesystem::exists(folderMeta->GetMetaPath()))
				{
					PX_CORE_WARN("Meta missing for Folder, creating: {0}", folderMeta->GetMetaPath().string());
					folderMeta->Serialize();
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