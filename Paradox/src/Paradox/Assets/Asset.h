#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Scene/UUID.h"

namespace Paradox
{
	enum class AssetType
	{
		Unknown,
		Texture2D,
		Scene
	};
	 
	class PARADOX_API Asset
	{
	//public:
	//	virtual AssetType GetAssetType() = 0;

	protected:
		UUID m_Handle = 0;
	};
}