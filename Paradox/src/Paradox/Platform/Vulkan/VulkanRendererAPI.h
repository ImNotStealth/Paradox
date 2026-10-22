#pragma once

#include "Paradox/Renderer/RendererAPI.h"
#include "Paradox/Renderer/Pipeline.h"
#include "Paradox/Platform/Vulkan/VulkanPipeline.h"

namespace Paradox
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		void Init() override {};
		void BeginFrame() override;
		void EndFrame() override;

		void BeginRenderPass(const Shared<Pipeline>& pipeline) override;
		void EndRenderPass() override;

		void DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer, const uint32_t indexCount = 0) override;
	
	private:
		Shared<VulkanPipeline> m_CurrentPipeline = nullptr;
	};
}