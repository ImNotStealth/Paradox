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

		Reference<Asset> CreateAsset() override;

		void SetWrap(TextureWrap wrap) { m_Wrap = wrap; }
		void SetFilter(TextureFilter filter) { m_MinFilter = filter; m_MagFilter = filter; }
		void SetAnisotropicFiltering(bool enabled) { m_AnisotropicFiltering = enabled; }

		TextureWrap GetWrap() { return m_Wrap; }
		TextureFilter GetFilter() { return m_MinFilter; }
		bool GetAnisotropicFiltering() { return m_AnisotropicFiltering; }

	private:
		TextureWrap m_Wrap = TextureWrap::Repeat;
		TextureFilter m_MinFilter = TextureFilter::Nearest;
		TextureFilter m_MagFilter = TextureFilter::Nearest;
		bool m_AnisotropicFiltering = true;
	};
}