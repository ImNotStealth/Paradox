#pragma once

#include "Paradox/Renderer/RendererAPI.h"

#include "Paradox/Platform/OpenGL/OpenGLPipeline.h"

namespace Paradox
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void BeginRenderPass(const Shared<Pipeline>& pipeline) override;
		void EndRenderPass() override;

		void DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer, const uint32_t indexCount = 0) override;
	
	private:
		static void MessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message, const void* userParam);

	private:
		Shared<OpenGLPipeline> m_CurrentPipeline = nullptr;
	};
}