#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/Pipeline.h"
#include "Paradox/Renderer/VertexBuffer.h"
#include "Paradox/Renderer/IndexBuffer.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class PARADOX_API Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void InitFrame();
		static void Begin(const glm::mat4& proj);
		static void End();

		static void DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const glm::vec4& tint);

		static uint32_t GetDrawCalls() { return s_Instance->m_DrawCalls; }
		static Shared<Framebuffer> GetFramebuffer() { return s_Instance->m_Pipeline->GetProperties().framebuffer; }
		static void SetFramebuffer(Shared<Framebuffer> framebuffer);

	private:
		void BeginBatch();
		void EndBatch();
		void GetOrAllocateVertexBuffer();

	private:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec4 color;
			glm::vec2 texCoord;
			float texIndex;
			float tilingFactor;
		};

		static const uint16_t c_MaxQuads = 3000;
		static const uint16_t c_MaxVertices = c_MaxQuads * 4;
		static const uint16_t c_MaxIndices = c_MaxQuads * 6;
		static const uint16_t c_MaxTextures = 16;

		struct VertexBufferData
		{
			Shared<VertexBuffer> buffer;
			uint16_t quadCount = 0;
			std::array<Shared<Texture2D>, c_MaxTextures> textures;
			uint16_t textureSlotIndex = 1;
		};

		std::vector<Vertex> m_Vertices;
		std::vector<VertexBufferData> m_VertexBufferPool;
		std::vector<Shared<UniformBufferSet>> m_UniformBufferSets;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;
		Shared<Texture2D> m_BlankTexture = nullptr;

		uint16_t m_ActiveVertexBufferIndex = 0;
		uint16_t m_BuffersUsedThisFrame = 0;
		uint16_t m_BufferStartIndex = 0;
		uint16_t m_UBSIndex = 0;
		uint32_t m_DrawCalls = 0;

		Shared<Shader> m_QuadShader = nullptr;
		Shared<Pipeline> m_Pipeline = nullptr;


		static Renderer2D* s_Instance;
	};
}