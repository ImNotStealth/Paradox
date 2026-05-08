#pragma once

#include "Paradox/Renderer/RendererAPI.h"
#include "Paradox/Renderer/Pipeline.h"

namespace Paradox
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void BeginRenderPass(const Shared<Pipeline>& pipeline) override;
		void EndRenderPass() override;

		void DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer) override;
	};
}