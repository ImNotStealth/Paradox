#include "pxpch.h"
#include "FolderMetadata.h"

#include "Paradox/Renderer/Texture.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define FOLDER_META_VERSION 1

namespace Paradox
{
	void FolderMetadata::Serialize()
	{
		PX_CORE_INFO("Serializing Meta for Folder: {0}", m_SourceAssetPath.string());

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
		writer.Int(FOLDER_META_VERSION);
		writer.Key("UUID");
		writer.String(m_AssetHandle.ToString());
		writer.Key("AssetType");
		writer.String(Asset::AssetTypeToString(m_AssetType));

		writer.Key("Color");
		writer.StartArray();
		writer.Double(m_Color.r);
		writer.Double(m_Color.g);
		writer.Double(m_Color.b);
		writer.EndArray();

		writer.EndObject();

		file << buffer.GetString();
		file.close();
	}

	void FolderMetadata::Deserialize()
	{
		PX_CORE_INFO("Deserializing Meta for Folder: {0}", m_SourceAssetPath.string());

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
		m_Color.r = document["Color"].GetArray()[0].GetFloat();
		m_Color.g = document["Color"].GetArray()[1].GetFloat();
		m_Color.b = document["Color"].GetArray()[2].GetFloat();
	}
}


