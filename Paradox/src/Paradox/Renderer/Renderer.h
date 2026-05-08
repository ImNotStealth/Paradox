#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/RendererAPI.h"
#include "Paradox/Renderer/Pipeline.h"
#include "Paradox/Renderer/VertexBuffer.h"
#include "Paradox/Renderer/IndexBuffer.h"

namespace Paradox
{
	class PARADOX_API Renderer
	{
	public:
		static void Init();

		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass(const Shared<Pipeline>& pipeline);
		static void EndRenderPass();

		static void DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer);

		static const uint32_t GetMaxFramesInFlight() { return 2; }

	private:
		static Unique<RendererAPI> s_RenderAPI;
	};
}