#pragma once

#include "Paradox/Renderer/ImGuiRenderer.h"

namespace Paradox
{
	class OpenGLImGuiRenderer : public ImGuiRenderer
	{
	public:
		void Init() override;
		void Shutdown() override;
		void BeginFrame() override;
		void EndFrame() override;

		void* GetContext() override;
	};
}