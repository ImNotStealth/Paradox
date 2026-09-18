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
			PX_CORE_ERROR("Failed to parse Project file: {0}", m_IndexFilePath.string());
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