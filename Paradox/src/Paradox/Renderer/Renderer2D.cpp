#include "pxpch.h"
#include "Renderer2D.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Renderer/Renderer.h"

namespace Paradox
{
	Renderer2D* Renderer2D::s_Instance = nullptr;

	void Renderer2D::Init()
	{
		PX_CORE_ASSERT(!s_Instance, "Renderer2D instance already exists.");

		s_Instance = new Renderer2D();
		s_Instance->m_QuadShader = Shader::Create("Quad2D", "Quad2D.vert", "Quad2D.frag");
		s_Instance->m_QuadShader->SetUniformBufferInput(0, s_Instance->m_CameraUBS, "Camera");
		s_Instance->m_QuadShader->BakeInput();

		FramebufferProperties framebufferProps = {};
		framebufferProps.swapchainTarget = false;
		framebufferProps.attachments = { { ImageFormat::RGBA }, { ImageFormat::Depth32F } };
		framebufferProps.clear = true;
		framebufferProps.width = Application::Get().GetWindow().GetWidth();
		framebufferProps.height = Application::Get().GetWindow().GetHeight();
		framebufferProps.debugName = "Renderer2D";
		Shared<Framebuffer> framebuffer = Framebuffer::Create(framebufferProps);

		PipelineProperties pipelineProps = {};
		pipelineProps.shader = s_Instance->m_QuadShader;
		pipelineProps.framebuffer = framebuffer;
		pipelineProps.debugName = "Renderer2D";
		pipelineProps.layout = {
			{ VertexBufferDataType::Float3 },
			{ VertexBufferDataType::Float4 }
		};
		pipelineProps.cullMode = CullMode::Back;
		s_Instance->m_Pipeline = Pipeline::Create(pipelineProps);

		s_Instance->m_VertexBuffer = VertexBuffer::Create(s_Instance->m_Vertices.data(), (uint32_t)(sizeof(Vertex) * s_Instance->m_Vertices.size()), VertexBufferUsage::Static);
		s_Instance->m_IndexBuffer = IndexBuffer::Create(s_Instance->m_Indices.data(), (uint32_t)s_Instance->m_Indices.size(), IndexBufferUsage::Static);
	}

	void Renderer2D::Shutdown()
	{
		PX_CORE_ASSERT(s_Instance != nullptr, "Renderer2D is not initialized!");

		delete s_Instance;
		s_Instance = nullptr;
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::mat4& proj)
	{
		s_Instance->m_CameraUBS->GetCurrent()->SetData(&proj, sizeof(glm::mat4));

		Renderer::BeginRenderPass(s_Instance->m_Pipeline);
		Renderer::DrawIndexed(s_Instance->m_VertexBuffer, s_Instance->m_IndexBuffer);
		Renderer::EndRenderPass();
	}

	void Renderer2D::SetFramebuffer(Shared<Framebuffer> framebuffer)
	{
		PipelineProperties& pipelineProps = s_Instance->m_Pipeline->GetProperties();
		pipelineProps.framebuffer = framebuffer;
		s_Instance->m_Pipeline = Pipeline::Create(pipelineProps);
	}
}