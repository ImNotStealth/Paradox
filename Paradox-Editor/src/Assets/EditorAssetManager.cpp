#include "EditorAssetManager.h"

namespace Paradox
{
	EditorAssetManager::~EditorAssetManager()
	{
		for (auto& [uuid, meta] : m_Metadatas)
			meta->Serialize();

		m_AssetIndex.Serialize();
	}

	void EditorAssetManager::InitAssetIndex()
	{
		ScanPath(m_AssetPath, true);
		m_AssetIndex.Serialize();
	}
}