#pragma once

#include "Panels/Panel.h"

#include <Paradox/Events/InputEvents.h>
#include <glm/glm.hpp>

namespace Paradox
{
	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel();

		void OnImGuiRender(bool* opened, float deltaTime) override;
		void OnEvent(Event& event) override;

		glm::vec2& GetViewportSize() { return m_ViewportSize; }
		inline ImGuiID GetDockID() { return m_WindowDockID; }

	private:
		void DrawSettings();
		bool OnInput(KeyPressEvent& event);

	private:
		class EditorApp& m_AppRef;
		glm::vec2 m_ViewportSize = { 0.f, 0.f };
		glm::vec2 m_ViewportBounds[2] = {};
		float m_GizmoSnap = 0.5f;
		int m_GizmoMode = 0;
		ImGuiID m_WindowDockID;
		
		bool m_CameraActive = false, m_OldCameraActive = false;
	};
}