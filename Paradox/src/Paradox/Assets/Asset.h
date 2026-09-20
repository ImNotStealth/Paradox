#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Scene/UUID.h"

namespace Paradox
{
	enum class AssetType
	{
		Unknown,
		Directory, // Only used for Metadata, does not have an Asset class linked to it.
		Texture2D,
		Scene
	};
	 
	class PARADOX_API Asset
	{
	public:
		static std::string AssetTypeToString(AssetType type);
		static AssetType StringToAssetType(const std::string& str);
		static AssetType GetTypeFromExtension(const std::string& extension);
	//	virtual AssetType GetAssetType() = 0;

	protected:
		UUID m_Handle;

	private:
		static std::unordered_map<std::string, AssetType> m_AssetExtensions;
	};
}