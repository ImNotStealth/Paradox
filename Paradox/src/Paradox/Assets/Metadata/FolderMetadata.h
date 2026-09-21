#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class FolderMetadata : public AssetMetadata
	{
	public:
		FolderMetadata(const std::filesystem::path& sourceAssetPath)
			: AssetMetadata(sourceAssetPath, AssetType::Directory) {}

		void Serialize() override;
		void Deserialize() override;

		Shared<Asset> CreateAsset() { PX_CORE_ASSERT(false, "Directories cannot be created as an Asset."); return nullptr; };

		inline glm::vec3& GetColor() { return m_Color; }

	private:
		glm::vec3 m_Color = { 1.f, 1.f, 1.f };
	};
}