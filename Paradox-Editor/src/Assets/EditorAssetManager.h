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

		bool Import(const std::filesystem::path& sourceFilePath, const std::filesystem::path& targetPath);
		void ScanPath(const std::filesystem::path& path, bool searchRecursive);

		void RenameAsset(const UUID& id, const std::string& newName);

	protected:
		void InitAssetIndex() override;
	};
}