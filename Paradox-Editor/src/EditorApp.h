#pragma once

#ifndef PX_INCLUDE_IMGUI
#error PX_INCLUDE_IMGUI must be enabled to compile the Editor.
#endif

#include <Paradox.h>

#include "Panels/PanelManager.h"
#include "Panels/ViewportPanel.h"
#include "Project/Project.h"
#include "Utils/EditorCamera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
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

		~EditorApp() { Shutdown(); }


		//TEMP
		Scene* GetScene() { return &m_Scene; }
		Entity GetSelectedEntity() { return m_SelectedEntity; }
		EditorCamera& GetCamera() { return m_Camera; }
		Shared<Framebuffer> GetCompositeFramebuffer() { return m_CompositeFramebuffer; }
		Unique<ViewportPanel>& GetViewportPanel() { return m_ViewportPanel; }
		inline Shared<AssetManager> GetEditorAssetManager() { return m_EditorAssetManager; }
		inline PanelManager& GetPanelManager() { return m_PanelManager; }
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

	private:
		void Init();
		void Shutdown();

		void OnEvent(Event& event) override;
		void OnUpdate(float deltaTime) override;
		void OnImGuiRender(float deltaTime) override;

		void RenderMenuBar();

	private:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec2 texCoord;
		};

		std::vector<Vertex> m_Vertices = {
			{{-0.5f, -0.5f, 0.0f},	{0.0f, 1.0f}},
			{{0.5f, -0.5f, 0.0f},	{1.0f, 1.0f}},
			{{0.5f, 0.5f, 0.0f},	{1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f},	{0.0f, 0.0f}},

			{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
			{{0.5f, -0.5f, -0.5f},	{1.0f, 1.0f}},
			{{0.5f, 0.5f, -0.5f},	{1.0f, 0.0f}},
			{{-0.5f, 0.5f, -0.5f},	{0.0f, 0.0f}}
		};

		std::vector<uint32_t> m_Indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
		Shared<Pipeline> m_ScenePipeline = nullptr;
		Shared<VertexBuffer> m_VertexBuffer = nullptr;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;
		EditorCamera m_Camera;

		Reference<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
		Reference<Texture2D> m_Texture = nullptr, m_TextureNiva = nullptr;

		Shared<Framebuffer> m_SceneFramebuffer = nullptr;
		Shared<Framebuffer> m_CompositeFramebuffer = nullptr;
		bool m_NeedResize = true;

		PanelManager m_PanelManager;
		Scene m_Scene;
		Entity m_SelectedEntity;

		Unique<ViewportPanel> m_ViewportPanel;

		Shared<AssetManager> m_EditorAssetManager;
	};
}