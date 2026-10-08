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
		Reference<class Texture2D> m_Texture;
		bool m_ReloadAsset = false;
		float m_Zoom = 10.0f;

		const float m_ZoomMin = 1.0f, m_ZoomMax = 10000.f;
		ImU32   ImageBgColor = IM_COL32(100, 100, 100, 255);
		ImU32   GridColor = IM_COL32(255, 255, 255, 100);
		bool    GridEnabled = true;
		bool    ViewReset = true;
		ImVec2  ViewOffset; // in image space
	};
}