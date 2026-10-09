#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

namespace Paradox
{
	class SceneMetadata : public AssetMetadata
	{
	public:
		SceneMetadata(const std::filesystem::path& sourceAssetPath)
			: AssetMetadata(sourceAssetPath, AssetType::Scene) {}

		Reference<Asset> CreateAsset() override;
	};
}