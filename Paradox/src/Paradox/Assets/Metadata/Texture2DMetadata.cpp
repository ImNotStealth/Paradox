#include "pxpch.h"
#include "Texture2DMetadata.h"

#include "Paradox/Renderer/Texture.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h>

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
		writer.String("FileVersion");
		writer.Int(TEXTURE2D_META_VERSION);
		writer.String("UUID");
		writer.Int(m_AssetHandle);
		writer.String("Width");
		writer.Int(texture->GetWidth());
		writer.String("Height");
		writer.Int(texture->GetHeight());
		writer.EndObject();

		file << buffer.GetString();
		file.close();
	}
}


