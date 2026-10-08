#pragma once

#include "Panels/Panel.h"

#include <Paradox/Assets/Metadata/AssetMetadata.h>

namespace Paradox
{
	class AssetEditorPanel : public Panel
	{
	public:
		AssetEditorPanel(const std::string& name, Shared<AssetMetadata> meta)
			: Panel(name), m_Metadata(meta) {}

		virtual void OnImGuiRender(bool* opened, float deltaTime) = 0;
		virtual void OnEvent(Event& event) {}
		virtual void OnClose() {}

		inline Shared<AssetMetadata> GetMetadata() const { return m_Metadata; }

	protected:
		Shared<AssetMetadata> m_Metadata;
	};
}