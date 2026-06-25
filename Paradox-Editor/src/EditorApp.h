#pragma once

#include <Paradox.h>

#include "Panels/PanelManager.h"
#include "Project/Project.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Paradox
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const WindowCreateProperties& windowProps, CommandLineParser args)
			: Application(windowProps, args)
		{
			Init();
		}

	private:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec3 color;
			glm::vec2 texCoord;
		};

		std::vector<Vertex> m_Vertices = {
			{{-0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f}},
			{{ 0.5f, -0.5f, 0.f}, {0.f, 1.f, 0.f}, {1.f, 1.f}},
			{{ 0.5f,  0.5f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f}},
			{{-0.5f,  0.5f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 0.f}}
		};

		std::vector<uint32_t> m_Indices = { 0, 1, 2, 2, 3, 0 };
		Shared<Pipeline> m_Pipeline = nullptr;
		Shared<VertexBuffer> m_VertexBuffer = nullptr;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;
		Camera m_Camera;

		Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
		Shared<UniformBufferSet> m_ColorUBS = UniformBufferSet::Create(sizeof(glm::vec4));
		glm::vec4 m_TestColor = glm::vec4(1.f, 0.f, 1.f, 1.f);
		Shared<Texture> m_Texture = nullptr, m_TextureNiva = nullptr;

		Shared<Framebuffer> m_Framebuffer = nullptr;
		bool m_NeedResize = true;
		glm::vec2 m_ViewportSize = { 0.f, 0.f };

		PanelManager m_PanelManager;

	private:
		void Init();

		void OnEvent(Event& event) override;
		void OnUpdate(float deltaTime) override;
		void OnImGuiRender(float deltaTime) override;

		void RenderMenuBar();
	};
}