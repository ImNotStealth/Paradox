#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/Pipeline.h"
#include "Paradox/Renderer/VertexBuffer.h"
#include "Paradox/Renderer/IndexBuffer.h"

namespace Paradox
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void BeginRenderPass(const Shared<Pipeline>& pipeline) = 0;
		virtual void EndRenderPass() = 0;

		virtual void DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer) = 0;

		static Unique<RendererAPI> Create();
	};
}