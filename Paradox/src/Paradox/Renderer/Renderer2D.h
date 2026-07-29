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

		static void Begin(const glm::mat4& proj);
		static void End();

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& tint);

		static Shared<Framebuffer> GetFramebuffer() { return s_Instance->m_Pipeline->GetProperties().framebuffer; }
		static void SetFramebuffer(Shared<Framebuffer> framebuffer);

	private:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec4 color;
		};

		uint32_t m_QuadCount = 0;
		std::vector<Vertex> m_Vertices;
		Shared<VertexBuffer> m_VertexBuffer = nullptr;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;

		Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
		Shared<Shader> m_QuadShader = nullptr;
		Shared<Pipeline> m_Pipeline = nullptr;

		static Renderer2D* s_Instance;
	};
}