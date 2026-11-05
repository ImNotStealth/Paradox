#include "EditorAssetManager.h"

#include <Paradox/Assets/Metadata/FolderMetadata.h>

namespace Paradox
{
	EditorAssetManager::~EditorAssetManager()
	{
		for (auto& [uuid, meta] : m_Metadatas)
			meta->Serialize();

		m_AssetIndex.Serialize();
	}

	void EditorAssetManager::InitAssetIndex()
	{
		ScanPath(m_AssetPath, true);
		m_AssetIndex.Serialize();
	}

	bool EditorAssetManager::Import(const std::filesystem::path& sourceFilePath, const std::filesystem::path& targetPath)
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

	void EditorAssetManager::ScanPath(const std::filesystem::path& path, bool searchRecursive)
	{
		if (!std::filesystem::exists(path))
			return;

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

	void EditorAssetManager::RenameAsset(const UUID& id, const std::string& newName)
	{
		const AssetIndex::IndexEntry& entry = m_AssetIndex.Get(id);
		const std::filesystem::path metaPath = m_AssetPath.parent_path() / entry.path;
		const std::filesystem::path resolvedPath = std::filesystem::path(metaPath).replace_extension();
		bool isDirectory = std::filesystem::is_directory(resolvedPath);

		std::filesystem::path newResolved = resolvedPath;
		newResolved.replace_filename(isDirectory ? newName : newName + resolvedPath.extension().string());

		if (std::filesystem::exists(newResolved))
		{
			PX_CORE_ERROR("Failed to rename '{0}': '{1}' already exists", entry.path.string(), newResolved.string());
			return;
		}

		std::filesystem::path newMeta = newResolved;
		newMeta += metaPath.extension();

		// Rename path in meta if loaded
		if (m_Metadatas.find(id) != m_Metadatas.end())
		{
			Reference<AssetMetadata> metadata = m_Metadatas[id];
			metadata->m_SourceAssetPath = newResolved;
		}

		if (isDirectory)
		{
			for (auto& [uuid, meta] : m_Metadatas)
			{
				if (uuid == id)
					continue;

				std::filesystem::path rel = meta->m_SourceAssetPath.lexically_relative(resolvedPath);
				if (rel.empty() || *rel.begin() == std::filesystem::path("..") || *rel.begin() == std::filesystem::path("."))
					continue;

				meta->m_SourceAssetPath = newResolved / rel;
			}
		}

		m_AssetIndex.Rename(id, newName);
		m_AssetIndex.Serialize();

		PX_CORE_INFO("{0}", newResolved.string());
		PX_CORE_INFO("{0}", newMeta.string());

		std::filesystem::rename(metaPath, newMeta);
		std::filesystem::rename(resolvedPath, newResolved);
	}
}