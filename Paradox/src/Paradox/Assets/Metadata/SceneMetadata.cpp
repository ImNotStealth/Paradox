#include "pxpch.h"
#include "SceneMetadata.h"

#include "Paradox/Scene/Scene.h"

namespace Paradox
{
	Reference<Asset> SceneMetadata::CreateAsset()
	{
		Reference<Scene> scene = CreateRef<Scene>();
		scene->Deserialize(m_SourceAssetPath);
		return scene;
	}
}