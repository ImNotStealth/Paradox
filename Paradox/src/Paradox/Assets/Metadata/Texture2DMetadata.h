#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class Texture2DMetadata : public AssetMetadata
	{
	public:
		Texture2DMetadata(std::filesystem::path sourceAssetPath)
			: AssetMetadata(sourceAssetPath, AssetType::Texture2D) {}

		void Serialize() override;
	};
}