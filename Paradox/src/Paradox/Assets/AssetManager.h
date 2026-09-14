#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Assets/Asset.h"
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

		static AssetManager* Get() { PX_CORE_ASSERT(s_Instance); return s_Instance; }

	private:
		template<typename T>
		void RegisterHandler(AssetType type, std::vector<std::string> fileExtensions);
		void LoadIndex();
		void UpdateMetadata();
		AssetType GetTypeFromExtension(const std::string& extension);

	protected:
		struct RegistryEntry
		{
			std::string path;
			AssetType assetType;
		};

		std::unordered_map<UUID, RegistryEntry> m_Index;
		std::unordered_map<std::string, AssetType> m_AssetExtensions;
		std::unordered_map<AssetType, std::function<Unique<AssetMetadata>(std::filesystem::path)>> m_MetaFactories;
		std::filesystem::path m_AssetPath;

		static AssetManager* s_Instance;
	};
}