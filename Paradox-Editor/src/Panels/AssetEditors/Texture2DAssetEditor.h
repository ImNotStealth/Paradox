#pragma once

#include "Panels/AssetEditors/AssetEditorPanel.h"

namespace Paradox
{
	class Texture2DAssetEditor : public AssetEditorPanel
	{
	public:
		Texture2DAssetEditor(Shared<AssetMetadata> meta);

		void OnImGuiRender(bool* opened, float deltaTime) override;

	private:
		class EditorApp& m_AppRef;
		Reference<class Texture2D> m_Texture;
		bool m_ReloadAsset = false, m_ResetView = true;
		bool m_DrawGrid = true;
		ImVec2 m_ViewOffset; // in image space
		float m_Zoom = 10.0f;

		const float m_ZoomMin = 1.0f, m_ZoomMax = 100.f;
	};
}