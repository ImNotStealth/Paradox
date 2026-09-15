#include "pxpch.h"
#include "Asset.h"

namespace Paradox
{
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
}