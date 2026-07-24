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

		static void BeginRenderPass(const Shared<Pipeline>& pipeline);
		static void EndRenderPass();

		static void DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer);

		static const uint32_t GetMaxFramesInFlight() { return 3; }

	private:
		static void BeginFrame();
		static void EndFrame();

	private:
		static Unique<RendererAPI> s_RenderAPI;
		friend class Application;
	};
}