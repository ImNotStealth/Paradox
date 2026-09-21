#include "pxpch.h"
#include "Texture2DMetadata.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#define TEXTURE2D_META_VERSION 1

namespace Paradox
{
	void Texture2DMetadata::Serialize()
	{
		PX_CORE_INFO("Serializing Meta for Texture2D: {0}", m_SourceAssetPath.string());

		std::filesystem::path metaPath = GetMetaPath();
		std::string fileName = metaPath.filename().string();

		std::ofstream file(metaPath.string().c_str());
		if (!file.is_open())
		{
			PX_ERROR("Failed to open Meta file: {0}", fileName);
			return;
		}

		Shared<Texture2D> texture = Texture2D::Create(m_SourceAssetPath.filename().string(), m_SourceAssetPath);

		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter writer(buffer);

		writer.StartObject();
		writer.Key("FileVersion");
		writer.Int(TEXTURE2D_META_VERSION);
		writer.Key("UUID");
		writer.String(m_AssetHandle.ToString());
		writer.Key("AssetType");
		writer.String(Asset::AssetTypeToString(m_AssetType));

		writer.Key("Wrap");
		writer.String(Texture::TextureWrapToString(m_Wrap));
		writer.Key("MinFilter");
		writer.String(Texture::TextureFilterToString(m_MinFilter));
		writer.Key("MagFilter");
		writer.String(Texture::TextureFilterToString(m_MagFilter));

		writer.Key("Width");
		writer.Int(texture->GetWidth());
		writer.Key("Height");
		writer.Int(texture->GetHeight());
		writer.EndObject();

		file << buffer.GetString();
		file.close();
	}

	void Texture2DMetadata::Deserialize()
	{
		PX_CORE_INFO("Deserializing Meta for Texture2D: {0}", m_SourceAssetPath.string());

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
		m_Wrap = Texture::StringToTextureWrap(document["Wrap"].GetString());
		m_MinFilter = Texture::StringToTextureFilter(document["MinFilter"].GetString());
		m_MagFilter = Texture::StringToTextureFilter(document["MagFilter"].GetString());
	}

	Shared<Asset> Texture2DMetadata::CreateAsset()
	{
		TextureProperties textureProps;
		textureProps.debugName = m_AssetHandle.ToString();
		textureProps.wrap = m_Wrap;
		textureProps.minFilter = m_MinFilter;
		textureProps.magFilter = m_MagFilter;
		return Texture2D::Create(textureProps, m_SourceAssetPath);
	}
}


