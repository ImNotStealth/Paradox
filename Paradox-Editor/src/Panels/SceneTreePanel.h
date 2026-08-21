#pragma once

#include "Panels/Panel.h"

#include <Paradox.h>
#include <Paradox/Events/InputEvents.h>

namespace Paradox
{
	class SceneTreePanel : public Panel
	{
	public:
		SceneTreePanel();

		void OnImGuiRender(bool* opened) override;
		void OnEvent(Event& event) override;

	private:
		void DrawEntry(Entity& entity);
		bool OnKeyPress(KeyPressEvent& event);

	private:
		class EditorApp& m_AppRef;
		bool m_WindowHovered = false;
		Scene* m_Scene = nullptr;
	};
}