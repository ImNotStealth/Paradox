#pragma once

#ifndef PX_INCLUDE_IMGUI
#error PX_INCLUDE_IMGUI must be enabled to compile the Editor.
#endif

#include <Paradox.h>

#include "Panels/PanelManager.h"
#include "Panels/ViewportPanel.h"
#include "Utils/EditorCamera.h"
#include "Assets/EditorAssetManager.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
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

		inline Reference<Scene> GetScene() { return m_Scene; }
		inline Entity GetSelectedEntity() { return m_SelectedEntity; }
		inline EditorCamera& GetCamera() { return m_Camera; }
		inline Shared<Framebuffer> GetCompositeFramebuffer() { return m_CompositeFramebuffer; }
		inline Unique<ViewportPanel>& GetViewportPanel() { return m_ViewportPanel; }
		inline Shared<EditorAssetManager> GetEditorAssetManager() { return m_EditorAssetManager; }
		inline PanelManager& GetPanelManager() { return m_PanelManager; }

		inline void SetScene(Reference<Scene> scene) { m_SelectedEntity = Entity(); m_Scene = scene; }
		inline void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

	private:
		void Init();
		void Shutdown();

		void OnEvent(Event& event) override;
		void OnUpdate(float deltaTime) override;
		void OnImGuiRender(float deltaTime) override;

		void RenderMenuBar();

	private:
		Shared<Pipeline> m_ScenePipeline = nullptr;
		Shared<VertexBuffer> m_VertexBuffer = nullptr;
		Shared<IndexBuffer> m_IndexBuffer = nullptr;
		EditorCamera m_Camera;

		struct GridSettings
		{
			glm::mat4 proj;
			glm::vec4 color;
			float scale;
			float size;
		};
		Reference<UniformBufferSet> m_GridUBS = UniformBufferSet::Create(sizeof(GridSettings));

		Shared<Framebuffer> m_SceneFramebuffer = nullptr;
		Shared<Framebuffer> m_CompositeFramebuffer = nullptr;
		bool m_NeedResize = true;

		PanelManager m_PanelManager;
		Reference<Scene> m_Scene = nullptr;
		Entity m_SelectedEntity;

		Unique<ViewportPanel> m_ViewportPanel;

		Shared<EditorAssetManager> m_EditorAssetManager;
	};
}