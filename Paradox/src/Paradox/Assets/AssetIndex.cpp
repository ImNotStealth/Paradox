#include "pxpch.h"
#include "AssetIndex.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define ASSET_INDEX_VERSION 1

namespace Paradox
{
	AssetIndex::AssetIndex(const std::filesystem::path& assetRootPath)
		: m_AssetRootPath(assetRootPath)
	{
		m_IndexFilePath = assetRootPath / ".." / "Index.pi";
	}

	void AssetIndex::Rename(const UUID& uuid, const std::string& newName)
	{
		PX_CORE_ASSERT(Contains(uuid));

		const std::filesystem::path oldPath = m_Index[uuid].path;
		const std::string metaExt = oldPath.extension().string();

		bool isDirectory = std::filesystem::is_directory(m_AssetRootPath.parent_path() / std::filesystem::path(oldPath).replace_extension());
		std::filesystem::path newPath = oldPath;
		if (isDirectory)
			newPath.replace_filename(newName + metaExt);
		else
			newPath.replace_filename(newName + oldPath.stem().extension().string() + metaExt);

		const std::filesystem::path newDiskPath = m_AssetRootPath.parent_path() / std::filesystem::path(newPath).replace_extension();
		if (std::filesystem::exists(newDiskPath))
		{
			PX_CORE_ERROR("Failed to rename '{0}': '{1}' already exists", oldPath.string(), newDiskPath.string());
			return;
		}

		const std::filesystem::path oldKey = std::filesystem::path(oldPath).replace_extension();
		const std::filesystem::path newKey = std::filesystem::path(newPath).replace_extension();

		if (isDirectory)
		{
			for (auto& [entryId, entry] : m_Index)
			{
				if (entryId == uuid)
					continue;

				std::filesystem::path rel = entry.path.lexically_relative(oldKey);
				if (rel.empty() || *rel.begin() == std::filesystem::path("..") || *rel.begin() == std::filesystem::path("."))
					continue;


				m_PathToUUID.erase(std::filesystem::path(entry.path).replace_extension());
				entry.path = newKey / rel;
				m_PathToUUID[std::filesystem::path(entry.path).replace_extension()] = uuid;
			}
		}

		m_PathToUUID.erase(oldKey);
		m_PathToUUID[newKey] = uuid;
		m_Index[uuid].path = newPath;
	}

	void AssetIndex::Delete(const UUID& uuid)
	{
		PX_CORE_ASSERT(Contains(uuid));

		const std::filesystem::path path = m_Index[uuid].path;
		const std::filesystem::path key = std::filesystem::path(path).replace_extension();
		bool isDirectory = std::filesystem::is_directory(m_AssetRootPath.parent_path() / key);

		if (isDirectory)
		{
			std::vector<UUID> childAssets;
			for (auto& [entryId, entry] : m_Index)
			{
				if (entryId == uuid)
					continue;

				std::filesystem::path rel = entry.path.lexically_relative(key);
				if (rel.empty() || *rel.begin() == std::filesystem::path("..") || *rel.begin() == std::filesystem::path("."))
					continue;

				m_PathToUUID.erase(std::filesystem::path(entry.path).replace_extension());
				childAssets.push_back(entryId);
			}

			for (const UUID& childId : childAssets)
				m_Index.erase(childId);
		}

		m_PathToUUID.erase(key);
		m_Index.erase(uuid);
	}

	void AssetIndex::Set(const UUID& uuid, const IndexEntry& entry)
	{
		PX_CORE_ASSERT(!Contains(uuid) && !Contains(entry.path));
		m_Index[uuid] = entry;
		m_PathToUUID[std::filesystem::path(entry.path).replace_extension()] = uuid;
	}

	void AssetIndex::Serialize()
	{
		std::ofstream file(m_IndexFilePath.string().c_str());
		if (!file.is_open())
		{
			PX_CORE_ERROR("Failed to open Index file: {0}", m_IndexFilePath.filename().string());
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(ASSET_INDEX_VERSION);
		writer.Key("Assets");
		writer.StartObject();

		for (const auto& [uuid, entry] : m_Index)
		{
			writer.Key(uuid.ToString().c_str());
			writer.StartObject();
			writer.Key("AssetType");
			writer.String(Asset::AssetTypeToString(entry.assetType));
			writer.Key("MetaPath");
			
			std::filesystem::path projectRoot = std::filesystem::absolute(m_AssetRootPath.parent_path());
			std::filesystem::path absEntry = (entry.path.is_absolute() ? entry.path : (projectRoot / entry.path)).lexically_normal();

			std::string metaPath = std::filesystem::relative(absEntry, projectRoot).string();
			std::replace(metaPath.begin(), metaPath.end(), '\\', '/');

			writer.String(metaPath);
			writer.EndObject();
		}

		writer.EndObject();
		writer.EndObject();

		file << buffer.GetString();
		file.close();
	}

	void AssetIndex::Deserialize()
	{
		if (!std::filesystem::exists(m_IndexFilePath))
		{
			PX_CORE_ERROR("Asset Index file does not exist: {0}", m_IndexFilePath.string());
			return;
		}

		std::ifstream file(m_IndexFilePath.string().c_str());
		if (!file.is_open())
		{
			PX_CORE_ERROR("Failed to open Asset Index file: {0}", m_IndexFilePath.string());
			return;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_CORE_ERROR("Failed to parse Asset Index file: {0}", m_IndexFilePath.string());
			return;
		}

		const rapidjson::Value& jsonVersion = document["FileVersion"];
		if (!document.HasMember("FileVersion") || !jsonVersion.IsInt())
		{
			PX_CORE_ERROR("Asset Index file is missing 'FileVersion'");
			return;
		}

		uint8_t version = jsonVersion.GetInt();
		if (version != ASSET_INDEX_VERSION)
		{
			PX_CORE_ERROR("Asset Index file version mismatch: {0} (expected {1})", version, ASSET_INDEX_VERSION);
			return;
		}

		const rapidjson::Value& jsonAssets = document["Assets"];
		if (!document.HasMember("Assets") || !jsonAssets.IsObject())
		{
			PX_CORE_ERROR("Asset Index file is missing 'Assets'");
			return;
		}

		for (auto it = jsonAssets.MemberBegin(); it != jsonAssets.MemberEnd(); ++it)
		{
			UUID uuid = UUID(it->name.GetString());
			std::string path = it->value["MetaPath"].GetString();
			m_Index[uuid] = { path, Asset::StringToAssetType(it->value["AssetType"].GetString())};
			m_PathToUUID[std::filesystem::path(path).replace_extension()] = uuid;
		}
	}
}