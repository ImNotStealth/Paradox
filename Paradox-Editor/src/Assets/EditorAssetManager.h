#pragma once

#include <Paradox/Assets/AssetManager.h>

namespace Paradox
{
	class EditorAssetManager : public AssetManager
	{
	public:
		EditorAssetManager(const std::filesystem::path& assetPath)
			: AssetManager(assetPath) {}

		~EditorAssetManager() override;

	protected:
		void InitAssetIndex() override;
	};
}