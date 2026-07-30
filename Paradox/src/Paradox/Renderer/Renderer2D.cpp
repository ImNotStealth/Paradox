#include "pxpch.h"
#include "Renderer2D.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

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

		const uint32_t maxQuads = 3000;
		const uint32_t maxVertices = maxQuads * 4;
		s_Instance->m_VertexBuffer = VertexBuffer::Create((sizeof(Vertex) * maxVertices), VertexBufferUsage::Dynamic);

		const uint32_t maxIndices = maxQuads * 6;
		//TODO: Allow for multiple vertex buffers if we consider that one is full
		//Issue right now is not Begin/EndRenderPass, it's the reuse if s_Instance->m_VertexBuffer within the same frame.
		std::vector<uint32_t> indices(maxIndices);
		uint32_t quadOffset = 0;
		for (uint32_t i = 0; i < maxIndices; i += 6)
		{
			indices[0 + i] = 0 + quadOffset;
			indices[1 + i] = 1 + quadOffset;
			indices[2 + i] = 2 + quadOffset;
			indices[3 + i] = 2 + quadOffset;
			indices[4 + i] = 3 + quadOffset;
			indices[5 + i] = 0 + quadOffset;
			quadOffset += 4;
		}
		s_Instance->m_IndexBuffer = IndexBuffer::Create(indices.data(), (uint32_t)indices.size(), IndexBufferUsage::Static);
	}

	void Renderer2D::Shutdown()
	{
		PX_CORE_ASSERT(s_Instance != nullptr, "Renderer2D is not initialized!");

		delete s_Instance;
		s_Instance = nullptr;
	}

	void Renderer2D::Begin(const glm::mat4& proj)
	{
		s_Instance->m_CameraUBS->GetCurrent()->SetData(&proj, sizeof(glm::mat4));
		s_Instance->m_Vertices.clear();
		s_Instance->m_QuadCount = 0;
	}

	void Renderer2D::End()
	{
		if (s_Instance->m_QuadCount == 0)
			return;

		s_Instance->m_VertexBuffer->SetData(s_Instance->m_Vertices.data(), (uint32_t)(sizeof(Vertex) * s_Instance->m_Vertices.size()));
		Renderer::BeginRenderPass(s_Instance->m_Pipeline);
		Renderer::DrawIndexed(s_Instance->m_VertexBuffer, s_Instance->m_IndexBuffer, s_Instance->m_QuadCount * 6);
		Renderer::EndRenderPass();
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& tint)
	{
		s_Instance->m_QuadCount++;
		s_Instance->m_Vertices.insert(s_Instance->m_Vertices.end(), {
			{ transform * glm::vec4{0.0f,  0.0f, 0.0f, 1.0f}, tint },
			{ transform * glm::vec4{1.0f,  0.0f, 0.0f, 1.0f}, tint },
			{ transform * glm::vec4{1.0f, -1.0f, 0.0f, 1.0f}, tint },
			{ transform * glm::vec4{0.0f, -1.0f, 0.0f, 1.0f}, tint }
		});
	}

	void Renderer2D::SetFramebuffer(Shared<Framebuffer> framebuffer)
	{
		PipelineProperties& pipelineProps = s_Instance->m_Pipeline->GetProperties();
		pipelineProps.framebuffer = framebuffer;
		s_Instance->m_Pipeline = Pipeline::Create(pipelineProps);
	}
}