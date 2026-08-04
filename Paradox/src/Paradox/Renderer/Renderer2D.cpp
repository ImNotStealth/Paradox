#include "pxpch.h"
#include "Renderer2D.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Paradox
{
	Renderer2D* Renderer2D::s_Instance = nullptr;

	//TODO: Need to add checks to make sure code is being called correctly
	void Renderer2D::Init()
	{
		PX_CORE_ASSERT(!s_Instance, "Renderer2D instance already exists.");

		s_Instance = new Renderer2D();

		ImageProperties imageProps = {};
		imageProps.usage = ImageUsage::Texture;
		imageProps.width = 1;
		imageProps.height = 1;
		imageProps.debugName = "Renderer2D Blank Texture";
		Shared<Image> blankImage = Image::Create(imageProps);
		uint32_t whiteTextureData = 0xFFFFFFFF;
		blankImage->SetData(&whiteTextureData, sizeof(uint32_t));

		TextureProperties textureProps = {};
		textureProps.debugName = imageProps.debugName;
		s_Instance->m_BlankTexture = Texture2D::CreateFromImage(textureProps, blankImage);

		s_Instance->m_QuadShader = Shader::Create("Quad2D", "Quad2D.vert", "Quad2D.frag");
		s_Instance->m_QuadShader->SetUniformBufferInput(0, s_Instance->m_CameraUBS, "Camera");
		for (size_t i = 0; i < s_Instance->c_MaxTextures; i++)
			s_Instance->m_QuadShader->SetTextureInput(1, s_Instance->m_BlankTexture, "Textures", i);
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
			{ VertexBufferDataType::Float4 },
			{ VertexBufferDataType::Float2 },
			{ VertexBufferDataType::Float },
			{ VertexBufferDataType::Float }
		};
		pipelineProps.cullMode = CullMode::Back;
		s_Instance->m_Pipeline = Pipeline::Create(pipelineProps);

		s_Instance->m_VertexBufferPool.emplace_back(VertexBufferData{ VertexBuffer::Create((sizeof(Vertex) * s_Instance->c_MaxVertices), VertexBufferUsage::Dynamic), 0 });

		const uint32_t maxIndices = s_Instance->c_MaxQuads * 6;
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

	void Renderer2D::InitFrame()
	{
		s_Instance->m_ActiveVertexBufferIndex = 0;
		s_Instance->m_BuffersUsed = 0;
	}

	void Renderer2D::Begin(const glm::mat4& proj)
	{
		s_Instance->m_CameraUBS->GetCurrent()->SetData(&proj, sizeof(glm::mat4));
		s_Instance->BeginBatch();
	}

	void Renderer2D::End()
	{
		s_Instance->EndBatch();

		for (uint16_t i = 0; i < s_Instance->m_BuffersUsed; i++)
		{
			const VertexBufferData& bufferData = s_Instance->m_VertexBufferPool[i];
			if (bufferData.quadCount == 0)
				continue;

			for (size_t i = 0; i < bufferData.textures.size(); i++)
				s_Instance->m_QuadShader->SetTextureInput(1, bufferData.textures[i] ? bufferData.textures[i] : s_Instance->m_BlankTexture, "Textures", i);

			//PX_CORE_WARN("Wrote {0} quads to VertexBuffer {1}", bufferData.quadCount, i);
			if (bufferData.quadCount * 6 > s_Instance->c_MaxIndices)
				PX_CORE_WARN("Renderer2D: VertexBuffer {0} has too many quads ({1})", i, bufferData.quadCount);
			Renderer::BeginRenderPass(s_Instance->m_Pipeline);
			Renderer::DrawIndexed(bufferData.buffer, s_Instance->m_IndexBuffer, bufferData.quadCount * 6);
			Renderer::EndRenderPass();
		}
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const glm::vec4& tint)
	{
		if (s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].quadCount + 1 > s_Instance->c_MaxQuads)
		{
			s_Instance->EndBatch();
			s_Instance->BeginBatch();
		}

		float textureIndex = 0.0f;
		VertexBufferData& activeBuffer = s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex];
		for (uint32_t i = 1; i < activeBuffer.textureSlotIndex; i++)
		{
			//TODO: This needs better checks, right now if the underlying image is the same but texture properties are different, they'll still act as the same.
			if (activeBuffer.textures[i] && *activeBuffer.textures[i]->GetImage() == *texture->GetImage())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (activeBuffer.textureSlotIndex >= s_Instance->c_MaxTextures)
			{
				s_Instance->EndBatch();
				s_Instance->BeginBatch();

				// Refresh reference in case BeginBatch changed m_ActiveVertexBufferIndex
				activeBuffer = s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex];
			}

			textureIndex = (float)activeBuffer.textureSlotIndex;
			activeBuffer.textures[activeBuffer.textureSlotIndex] = texture;
			activeBuffer.textureSlotIndex++;
		}

		activeBuffer.quadCount++;

		const float tilingFactor = 1.0f;
		s_Instance->m_Vertices.insert(s_Instance->m_Vertices.end(), {
			{ transform * glm::vec4{0.0f,  0.0f, 0.0f, 1.0f}, tint, {0.f, 0.f}, textureIndex, tilingFactor },
			{ transform * glm::vec4{1.0f,  0.0f, 0.0f, 1.0f}, tint, {1.f, 0.f}, textureIndex, tilingFactor },
			{ transform * glm::vec4{1.0f, -1.0f, 0.0f, 1.0f}, tint, {1.f, 1.f}, textureIndex, tilingFactor },
			{ transform * glm::vec4{0.0f, -1.0f, 0.0f, 1.0f}, tint, {0.f, 1.f}, textureIndex, tilingFactor }
		});
	}

	void Renderer2D::SetFramebuffer(Shared<Framebuffer> framebuffer)
	{
		PipelineProperties& pipelineProps = s_Instance->m_Pipeline->GetProperties();
		pipelineProps.framebuffer = framebuffer;
		s_Instance->m_Pipeline = Pipeline::Create(pipelineProps);
	}

	void Renderer2D::BeginBatch()
	{
		s_Instance->m_Vertices.clear();
		s_Instance->GetOrAllocateVertexBuffer();

		s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].textureSlotIndex = 1;
	}

	void Renderer2D::EndBatch()
	{
		if (s_Instance->m_Vertices.empty())
			return;

		Shared<VertexBuffer> buffer = s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].buffer;
		buffer->SetData(s_Instance->m_Vertices.data(), (uint32_t)(sizeof(Vertex) * s_Instance->m_Vertices.size()));
	}

	void Renderer2D::GetOrAllocateVertexBuffer()
	{
		uint16_t nextIndex = s_Instance->m_BuffersUsed;

		if (nextIndex >= s_Instance->m_VertexBufferPool.size())
		{
			VertexBufferData bufferData;
			bufferData.buffer = VertexBuffer::Create((sizeof(Vertex) * s_Instance->c_MaxVertices), VertexBufferUsage::Dynamic);
			bufferData.quadCount = 0;
			bufferData.textures[0] = s_Instance->m_BlankTexture;
			bufferData.textureSlotIndex = 1;
			s_Instance->m_VertexBufferPool.emplace_back(bufferData);	
		}

		s_Instance->m_ActiveVertexBufferIndex = nextIndex;
		s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].quadCount = 0;
		s_Instance->m_BuffersUsed++;
	}
}