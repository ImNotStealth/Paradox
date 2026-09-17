#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/AssetIndex.h"
#include "Paradox/Assets/Metadata/AssetMetadata.h"

namespace Paradox
{
	//TODO: This is all temporary, ideally some functions should be split into Editor and Runtime.
	class PARADOX_API AssetManager
	{
	public:
		AssetManager(std::filesystem::path assetPath);

		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

		Shared<Asset> GetAsset(UUID id);

		inline AssetIndex& GetAssetIndex() { return m_AssetIndex; }
		static AssetManager* Get() { PX_CORE_ASSERT(s_Instance); return s_Instance; }

	private:
		template<typename T>
		void RegisterMetadata(AssetType type, std::vector<std::string> fileExtensions);
		void CreateMissingMetadata();
		AssetType GetTypeFromExtension(const std::string& extension);

	protected:
		std::unordered_map<std::string, AssetType> m_AssetExtensions;
		std::unordered_map<AssetType, std::function<Unique<AssetMetadata>(std::filesystem::path)>> m_MetaFactories;
		std::filesystem::path m_AssetPath;
		AssetIndex m_AssetIndex;

		static AssetManager* s_Instance;

		friend class AssetIndexPanel;
	};
}