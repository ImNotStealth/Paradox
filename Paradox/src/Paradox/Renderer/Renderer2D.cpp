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

		s_Instance->m_UniformBufferSets.emplace_back(UniformBufferSet::Create(sizeof(glm::mat4)));

		s_Instance->m_QuadShader = Shader::Create("Quad2D", "Quad2D.vert", "Quad2D.frag");
		s_Instance->m_QuadShader->SetUniformBufferInput(0, s_Instance->m_UniformBufferSets[0], "Camera");
		for (size_t i = 0; i < c_MaxTextures; i++)
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
		pipelineProps.cullMode = CullMode::None;
		s_Instance->m_Pipeline = Pipeline::Create(pipelineProps);

		s_Instance->m_VertexBufferPool.emplace_back(VertexBufferData{ VertexBuffer::Create((sizeof(Vertex) * c_MaxVertices), VertexBufferUsage::Dynamic), 0 });

		const uint32_t maxIndices = c_MaxQuads * 6;
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
		s_Instance->m_BuffersUsedThisFrame = 0;
		s_Instance->m_UBSIndex = 0;
		s_Instance->m_Stats = {};
	}

	void Renderer2D::Begin(const glm::mat4& proj)
	{
		if (s_Instance->m_UBSIndex >= s_Instance->m_UniformBufferSets.size())
		{
			s_Instance->m_UniformBufferSets.emplace_back(UniformBufferSet::Create(sizeof(glm::mat4)));
			PX_CORE_TRACE("Renderer2D: Allocated new UBS, Size: {0}, Index: {1}", s_Instance->m_UniformBufferSets.size(), s_Instance->m_UBSIndex);
		}

		s_Instance->m_UniformBufferSets[s_Instance->m_UBSIndex]->GetCurrent()->SetData(&proj, sizeof(glm::mat4));
		s_Instance->m_UBSIndex++;
		s_Instance->BeginBatch();
		s_Instance->m_BufferStartIndex = s_Instance->m_ActiveVertexBufferIndex;
	}

	void Renderer2D::End()
	{
		s_Instance->EndBatch();

		bool beganRenderPass = false;

		for (uint16_t i = s_Instance->m_BufferStartIndex; i <= s_Instance->m_ActiveVertexBufferIndex; i++)
		{
			const VertexBufferData& bufferData = s_Instance->m_VertexBufferPool[i];
			if (bufferData.quadCount == 0)
				continue;

			s_Instance->m_QuadShader->SetUniformBufferInput(0, s_Instance->m_UniformBufferSets[s_Instance->m_UBSIndex - 1], "Camera");

			for (size_t j = 0; j < bufferData.textures.size(); j++)
				s_Instance->m_QuadShader->SetTextureInput(1, bufferData.textures[j] ? bufferData.textures[j] : s_Instance->m_BlankTexture, "Textures", j);

			//PX_CORE_WARN("Wrote {0} quads to VertexBuffer {1}", bufferData.quadCount, i);
			if (bufferData.quadCount * 6 > c_MaxIndices)
				PX_CORE_WARN("Renderer2D: VertexBuffer {0} has too many quads ({1})", i, bufferData.quadCount);
			
			if (!beganRenderPass)
			{
				Renderer::BeginRenderPass(s_Instance->m_Pipeline);
				beganRenderPass = true;
			}
			Renderer::DrawIndexed(bufferData.buffer, s_Instance->m_IndexBuffer, bufferData.quadCount * 6);
			s_Instance->m_Stats.drawCalls++;
		}
		
		if (beganRenderPass)
			Renderer::EndRenderPass();
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tint, float tilingFactor, glm::vec2 uv0, glm::vec2 uv1)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		DrawQuad(transform, texture, tint, tilingFactor, uv0, uv1);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const glm::vec4& tint, float tilingFactor, glm::vec2 uv0, glm::vec2 uv1)
	{
		if (s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].quadCount + 1 > c_MaxQuads)
		{
			s_Instance->EndBatch();
			s_Instance->BeginBatch();
		}

		float textureIndex = 0.0f;
		VertexBufferData* activeBuffer = &s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex];
		for (uint32_t i = 1; i < activeBuffer->textureSlotIndex; i++)
		{
			//TODO: This needs better checks, right now if the underlying image is the same but texture properties are different, they'll still act as the same.
			if (activeBuffer->textures[i] && *activeBuffer->textures[i]->GetImage() == *texture->GetImage())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (activeBuffer->textureSlotIndex >= c_MaxTextures)
			{
				s_Instance->EndBatch();
				s_Instance->BeginBatch();

				// Refresh reference in case BeginBatch changed m_ActiveVertexBufferIndex
				activeBuffer = &s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex];
			}

			textureIndex = (float)activeBuffer->textureSlotIndex;
			activeBuffer->textures[activeBuffer->textureSlotIndex] = texture;
			activeBuffer->textureSlotIndex++;
		}

		activeBuffer->quadCount++;
		s_Instance->m_Stats.quadCount++;

		glm::vec2 texCoords[] =
		{
			uv0, { uv1.x, uv0.y },
			uv1, { uv0.x, uv1.y }
		};

		s_Instance->m_Vertices.insert(s_Instance->m_Vertices.end(), {
			{ transform * glm::vec4{0.0f,  0.0f, 0.0f, 1.0f}, tint, texCoords[0], textureIndex, tilingFactor},
			{ transform * glm::vec4{1.0f,  0.0f, 0.0f, 1.0f}, tint, texCoords[1], textureIndex, tilingFactor },
			{ transform * glm::vec4{1.0f, -1.0f, 0.0f, 1.0f}, tint, texCoords[2], textureIndex, tilingFactor },
			{ transform * glm::vec4{0.0f, -1.0f, 0.0f, 1.0f}, tint, texCoords[3], textureIndex, tilingFactor }
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
		if (s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].quadCount == 0)
			return;

		uint16_t nextIndex = s_Instance->m_BuffersUsedThisFrame;
		if (nextIndex >= s_Instance->m_VertexBufferPool.size())
		{
			VertexBufferData bufferData;
			bufferData.buffer = VertexBuffer::Create((sizeof(Vertex) * c_MaxVertices), VertexBufferUsage::Dynamic);
			bufferData.quadCount = 0;
			bufferData.textures[0] = s_Instance->m_BlankTexture;
			bufferData.textureSlotIndex = 1;
			s_Instance->m_VertexBufferPool.emplace_back(bufferData);	
		}

		s_Instance->m_ActiveVertexBufferIndex = nextIndex;
		s_Instance->m_VertexBufferPool[s_Instance->m_ActiveVertexBufferIndex].quadCount = 0;
		s_Instance->m_BuffersUsedThisFrame++;
	}
}