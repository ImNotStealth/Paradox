#include "pxpch.h"
#include "StatisticsPanel.h"

#include "EditorApp.h"

#include <Paradox.h>
#include <imgui.h>

namespace Paradox
{
	StatisticsPanel::StatisticsPanel()
		: Panel("Statistics"), m_AppRef(static_cast<class EditorApp&>(Application::Get())) {}

	void StatisticsPanel::OnImGuiRender(bool* opened)
	{
		PX_PROFILE_FUNCTION();

		ImGui::Begin("Statistics", opened);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("General");
		ImGui::PopFont();
		ImGui::Text("Viewport Size: %.0f x %.0f", m_AppRef.GetViewportSize().x, m_AppRef.GetViewportSize().y);
		ImGui::Text("Graphics API: %s", GraphicsContext::GraphicsAPIToString(GraphicsContext::GetGraphicsAPI()).c_str());
		ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::CollapsingHeader("Renderer2D", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("\tDraw Calls: %d", Renderer2D::GetStatistics().drawCalls);
			ImGui::Text("\tQuad Count: %d", Renderer2D::GetStatistics().quadCount);
		}

		ImGui::End();
	}
}