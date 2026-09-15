#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class FolderMetadata : public AssetMetadata
	{
	public:
		FolderMetadata(std::filesystem::path sourceAssetPath)
			: AssetMetadata(sourceAssetPath, AssetType::Directory) {}

		void Serialize() override;

	private:
		glm::vec3 m_Color = { 1.f, 1.f, 1.f };
	};
}