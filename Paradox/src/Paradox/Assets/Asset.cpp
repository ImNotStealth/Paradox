#include "pxpch.h"
#include "Asset.h"

namespace Paradox
{
    std::unordered_map<std::string, AssetType> Asset::m_AssetExtensions =
    {
        { ".png", AssetType::Texture2D },
        { ".jpg", AssetType::Texture2D },
        { ".jpeg", AssetType::Texture2D },
    };

    std::string Asset::AssetTypeToString(AssetType type)
    {
        switch (type)
        {
            case AssetType::Directory: return "Directory";
            case AssetType::Texture2D: return "Texture2D";
            case AssetType::Unknown: return "Unknown";
        }
        PX_CORE_ASSERT(false, "Unknown AssetType");
        return "";
    }

    AssetType Asset::StringToAssetType(const std::string& str)
    {
        if (str == "Directory") return AssetType::Directory;
        if (str == "Texture2D") return AssetType::Texture2D;
        if (str == "Unknown") return AssetType::Unknown;

        PX_CORE_ASSERT(false, "Invalid AssetType");
        return AssetType::Unknown;
    }

    AssetType Asset::GetTypeFromExtension(const std::string& extension)
    {
        if (m_AssetExtensions.find(extension) == m_AssetExtensions.end())
            return AssetType::Unknown;

        return m_AssetExtensions[extension];
    }
}