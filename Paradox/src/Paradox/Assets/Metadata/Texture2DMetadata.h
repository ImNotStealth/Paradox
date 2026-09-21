#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"
#include "Paradox/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class Texture2DMetadata : public AssetMetadata
	{
	public:
		Texture2DMetadata(const std::filesystem::path& sourceAssetPath)
			: AssetMetadata(sourceAssetPath, AssetType::Texture2D) {}

		void Serialize() override;
		void Deserialize() override;

		Shared<Asset> CreateAsset() override;

	private:
		TextureWrap m_Wrap = TextureWrap::Repeat;
		TextureFilter m_MinFilter = TextureFilter::Nearest;
		TextureFilter m_MagFilter = TextureFilter::Nearest;
	};
}