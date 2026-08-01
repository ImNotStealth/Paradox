#include "pxpch.h"
#include "Renderer.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Renderer/Renderer2D.h"

namespace Paradox
{
	Unique<RendererAPI> Renderer::s_RenderAPI = nullptr;
	
	void Renderer::Init()
	{
		PX_CORE_INFO("Init Renderer");
		s_RenderAPI = RendererAPI::Create();
		s_RenderAPI->Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		PX_CORE_INFO("Shutdown Renderer");
		Renderer2D::Shutdown();
	}

	void Renderer::BeginFrame()
	{
		Application::Get().GetWindow().GetSwapChain()->Begin();
		Renderer2D::InitFrame();
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

	void Renderer::DrawIndexed(const Shared<VertexBuffer> vertexBuffer, const Shared<IndexBuffer> indexBuffer, const uint32_t indexCount)
	{
		s_RenderAPI->DrawIndexed(vertexBuffer, indexBuffer, indexCount);
	}
}