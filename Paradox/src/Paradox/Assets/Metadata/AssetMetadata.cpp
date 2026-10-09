#include "pxpch.h"
#include "AssetMetadata.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define GENERIC_META_VERSION 1

namespace Paradox
{
	void AssetMetadata::Serialize()
	{
		PX_CORE_INFO("Serializing Meta for Generic: {0}", m_SourceAssetPath.string());

		std::filesystem::path metaPath = GetMetaPath();
		std::string fileName = metaPath.filename().string();

		std::ofstream file(metaPath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open Meta file: {0}", fileName);
			return;
		}

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(GENERIC_META_VERSION);
		writer.Key("UUID");
		writer.String(m_AssetHandle.ToString());
		writer.Key("AssetType");
		writer.String(Asset::AssetTypeToString(m_AssetType));
		writer.EndObject();

		file << buffer.GetString();
		file.close();
	}

	void AssetMetadata::Deserialize()
	{
		PX_CORE_INFO("Deserializing Meta for Generic: {0}", m_SourceAssetPath.string());

		std::filesystem::path filePath = GetMetaPath();

		if (!std::filesystem::exists(filePath))
		{
			PX_ERROR("Meta file does not exist: {0}", filePath.string());
			return;
		}

		std::ifstream file(filePath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open Meta file: {0}", filePath.string());
			return;
		}

		rapidjson::IStreamWrapper streamWrapper(file);
		rapidjson::Document document;
		document.ParseStream(streamWrapper);

		if (document.HasParseError())
		{
			PX_ERROR("Failed to parse Meta file: {0}", filePath.string());
			return;
		}

		m_AssetHandle = UUID(document["UUID"].GetString());
	}
}
