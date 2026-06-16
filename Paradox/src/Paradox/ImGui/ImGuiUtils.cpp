#include "pxpch.h"
#include "ImGuiUtils.h"

#include "Paradox/Renderer/GraphicsContext.h"

#include <imgui.h>

namespace Paradox
{
	void ImGuiUtils::Image(uint64_t imageId, ImVec2 size)
	{
		bool flipY = GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::OpenGL;
		ImGui::Image(imageId, size, { 0.f, flipY ? 1.f : 0.f }, { 1.f, flipY ? 0.f : 1.f });
	}
}