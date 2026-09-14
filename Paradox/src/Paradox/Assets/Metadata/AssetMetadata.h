#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Asset.h"

namespace Paradox
{
	class AssetMetadata
	{
	public:
		AssetMetadata(std::filesystem::path sourceAssetPath, AssetType type = AssetType::Unknown)
			: m_SourceAssetPath(sourceAssetPath), m_AssetType(type) {}

		virtual void Serialize() = 0;

		virtual std::filesystem::path GetMetaPath()
		{
			std::filesystem::path path = m_SourceAssetPath;
			path += ".pm";
			return path;
		}

	protected:
		std::filesystem::path m_SourceAssetPath;
		AssetType m_AssetType;
		UUID m_AssetHandle;
	};
}