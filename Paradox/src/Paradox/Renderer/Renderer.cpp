#include "pxpch.h"
#include "Renderer.h"

#include "Paradox/Core/Application.h"

namespace Paradox
{
	Unique<RendererAPI> Renderer::s_RenderAPI = nullptr;
	
	void Renderer::Init()
	{
		s_RenderAPI = RendererAPI::Create();
		s_RenderAPI->Init();
	}

	void Renderer::BeginFrame()
	{
		Application::Get().GetWindow().GetSwapChain()->Begin();
	}

	void Renderer::EndFrame()
	{
		Application::Get().GetWindow().GetSwapChain()->End();
	}

	void Renderer::BeginRenderPass(const Shared<Pipeline>& pipeline)
	{
		s_RenderAPI->BeginRenderPass(pipeline);
	}

	void Renderer::EndRenderPass()
	{
		s_RenderAPI->EndRenderPass();
	}

	void Renderer::DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer)
	{
		s_RenderAPI->DrawIndexed(vertexBuffer, indexBuffer);
	}
}