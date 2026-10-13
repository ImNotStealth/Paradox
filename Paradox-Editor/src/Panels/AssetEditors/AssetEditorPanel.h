#pragma once

#include "Panels/Panel.h"

#include <Paradox/Assets/Metadata/AssetMetadata.h>

namespace Paradox
{
	class AssetEditorPanel : public Panel
	{
	public:
		AssetEditorPanel(const std::string& name, Reference<AssetMetadata> meta)
			: Panel(name), m_Metadata(meta) {}

		virtual void OnImGuiRender(bool* opened, float deltaTime) = 0;
		virtual void OnEvent(Event& event) {}
		virtual void OnClose() {}

		inline Reference<AssetMetadata> GetMetadata() const { return m_Metadata; }

	protected:
		Reference<AssetMetadata> m_Metadata;
	};
}