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

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& tint);

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
		};

		struct VertexBufferData
		{
			Shared<VertexBuffer> buffer;
			uint16_t quadCount = 0;
		};
		std::vector<Vertex> m_Vertices;
		std::vector<VertexBufferData> m_VertexBufferPool;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;

		uint16_t m_ActiveVertexBufferIndex = 0;
		uint16_t m_BuffersUsed = 0;

		Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
		Shared<Shader> m_QuadShader = nullptr;
		Shared<Pipeline> m_Pipeline = nullptr;

		//TEMP
		Shared<Texture2D> m_BlankTexture = nullptr;

		uint16_t c_MaxQuads = 3000;
		uint16_t c_MaxVertices = c_MaxQuads * 4;
		uint16_t c_MaxIndices = c_MaxQuads * 6;

		static Renderer2D* s_Instance;
	};
}