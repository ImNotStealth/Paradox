#include "EditorApp.h"

#include "Panels/Popups/CreateProjectPanel.h"
#include "Panels/Popups/AboutPanel.h"
#include "Panels/Popups/ProjectSettingsPanel.h"
#ifdef PX_PLATFORM_LINUX
#include "Platform/Linux/ParseDumpPanel.h"
#endif

#include <Paradox/Assets/Metadata/SceneMetadata.h>
#include <Paradox/Core/FileSystem.h>
#include <Paradox/ImGui/ImGuiUtils.h>
#include <ImGuizmo.h>

namespace Paradox
{
	void EditorApp::Init()
	{
		ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());
		ImGuizmo::SetImGuiContext((ImGuiContext*)GetImGuiContext());

		//Editor
		m_EditorAssetManager = CreateShared<AssetManager>(std::filesystem::current_path() / "Assets");
		
		m_PanelManager.RegisterPanels();

		m_ViewportPanel = CreateUnique<ViewportPanel>();

		m_Texture = Texture2D::Create("Test Texture", "Assets/Textures/texture.jpg");

		TextureProperties nivaProps = {};
		nivaProps.debugName = "Niva";
		nivaProps.magFilter = TextureFilter::Nearest;
		m_TextureNiva = Texture2D::Create(nivaProps, "Assets/Textures/Controls.png");

		Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
		shader->SetUniformBufferInput(0, m_CameraUBS, "Camera");
		shader->SetTextureInput(1, m_Texture, "TextureArrayTest", 0);
		shader->SetTextureInput(1, m_TextureNiva, "TextureArrayTest", 1);
		shader->BakeInput();

		FramebufferProperties framebufferProps = {};
		framebufferProps.width = 1280;
		framebufferProps.height = 720;
		framebufferProps.swapchainTarget = false;
		framebufferProps.attachments = { { ImageFormat::RGBA }, { ImageFormat::Depth32F } };
		framebufferProps.debugName = "Scene Framebuffer";
		m_SceneFramebuffer = Framebuffer::Create(framebufferProps);

		framebufferProps.clear = false;
		framebufferProps.debugName = "Composite Framebuffer";
		framebufferProps.attachments = {
	            { ImageFormat::RGBA,     m_SceneFramebuffer->GetAttachmentImage(0) },
				{ ImageFormat::Depth32F, m_SceneFramebuffer->GetAttachmentImage(1) }
		};
		m_CompositeFramebuffer = Framebuffer::Create(framebufferProps);

		PipelineProperties scenePipelineProps = {};
		scenePipelineProps.shader = shader;
		scenePipelineProps.framebuffer = m_SceneFramebuffer;
		scenePipelineProps.debugName = "Scene Pipeline";
		scenePipelineProps.layout = {
			{ VertexBufferDataType::Float3 },
			{ VertexBufferDataType::Float2 }
		};
		scenePipelineProps.cullMode = CullMode::None;
		m_ScenePipeline = Pipeline::Create(scenePipelineProps);

		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

		if (GetCommandLineArgs().HasFlag("project"))
		{
			std::string projectPath = GetCommandLineArgs().GetRequired<std::string>("project");
			Project::SetActive(CreateShared<Project>(projectPath));
		}

		// Maximize the window here instead of in CreateApplication to let the renderer start up (to prevent the white fullscreen)
		GetWindow().Maximize();
		Renderer2D::SetFramebuffer(m_CompositeFramebuffer);
	}

	void EditorApp::Shutdown()
	{
		PX_INFO("Shutting down Editor.");

		if (m_Scene)
		{
			Reference<SceneMetadata> metadata = AssetManager::Get()->GetMetadata<SceneMetadata>(m_Scene->GetAssetHandle());
			m_Scene->Serialize(metadata->GetMetaPath().replace_extension());
		}

		if (Project::GetActive())
			Project::GetActive()->Serialize();
		Project::SetActive(nullptr); // Unload Project now so AssetManager etc can get destroyed too
		ImGuizmo::SetImGuiContext(nullptr);
	}

	void EditorApp::OnEvent(Event& event)
	{
		m_PanelManager.OnEvent(event);

		if (m_ViewportPanel)
			m_ViewportPanel->OnEvent(event);

		if (event.GetEventType() == EventType::WindowResize)
			m_NeedResize = true;

		if (event.GetEventType() == EventType::ProjectChanged && Project::GetActive())
		{
			const ProjectProperties& projectProperties = Project::GetActive()->GetProperties();
			Application::Get().GetWindow().SetTitle("Paradox Editor - " + projectProperties.name);

			PX_INFO(projectProperties.startScene.ToString());
			if (projectProperties.startScene != UUID::EMPTY)
			{
				Reference<Scene> scene = AssetManager::Get()->GetAsset<Scene>(projectProperties.startScene);
				if (scene)
					SetScene(scene);
			}
		}
	}

	void EditorApp::OnUpdate(float deltaTime)
	{
		const FramebufferProperties& fbProps = m_SceneFramebuffer->GetProperties();
		glm::vec2& viewportSize = m_ViewportPanel->GetViewportSize();
		if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && (fbProps.width != viewportSize.x || fbProps.height != viewportSize.y))
		{
			m_SceneFramebuffer->Resize(viewportSize.x, viewportSize.y);
			m_CompositeFramebuffer->Resize(viewportSize.x, viewportSize.y);
			m_Camera.SetViewportSize(viewportSize.x, viewportSize.y);
			m_NeedResize = false;
		}

		m_CameraUBS->GetCurrent()->SetData(&m_Camera.GetViewProjection(), sizeof(glm::mat4));

		Renderer::BeginRenderPass(m_ScenePipeline);
		Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
		Renderer::EndRenderPass();

		if (m_Scene)
			m_Scene->Update(m_Camera.GetViewProjection(), deltaTime);

		if (AssetManager::IsValid())
			AssetManager::Get()->Update();
	}

	void EditorApp::OnImGuiRender(float deltaTime)
	{
		ImGuizmo::BeginFrame();
		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID("EditorDockSpace"), ImVec2(0.0f, 0.0f), dockspace_flags);
		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			RenderMenuBar();
			ImGui::EndMenuBar();
		}

		m_ViewportPanel->OnImGuiRender(nullptr, deltaTime);

		ImGui::Begin("Settings");
		bool isVsync = GetWindow().IsVSync();
		if (ImGui::Checkbox("Toggle VSync", &isVsync))
			GetWindow().SetVSync(isVsync);
		ImGui::End();

		m_PanelManager.OnImGuiRender(deltaTime);

		ImGui::End(); //Dockspace

		ImGui::ShowDemoWindow();
	}

	void EditorApp::RenderMenuBar()
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::SeparatorText("Project");
			if (ImGui::MenuItem("Open Project..."))
			{
				std::filesystem::path path = FileSystem::SelectFile("Select Project file", "Paradox Project|*.px");
				if (!path.empty())
					Project::SetActive(CreateShared<Project>(path));
			}

			if (ImGui::MenuItem("Project Settings..."))
				m_PanelManager.OpenPopup<ProjectSettingsPanel>();

			if (ImGui::MenuItem("Create new Project..."))
				m_PanelManager.OpenPopup<CreateProjectPanel>();

			ImGui::SeparatorText("Save");
			if (ImGui::MenuItem("Save Scene") && m_Scene)
			{
				Reference<SceneMetadata> metadata = AssetManager::Get()->GetMetadata<SceneMetadata>(m_Scene->GetAssetHandle());
				m_Scene->Serialize(metadata->GetMetaPath().replace_extension());
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
				Application::Get().Stop();

			ImGui::EndMenu();
		}

		m_PanelManager.RenderMenuBar();

		if (ImGui::BeginMenu("Tools"))
		{
#ifdef PX_PLATFORM_LINUX
			bool enabled = true;
#else
			bool enabled = false;
#endif
			if (!enabled)
				ImGui::BeginDisabled();
			ImGui::SeparatorText("PS Vita");
			if (ImGui::MenuItem("Parse dump..."))
			{
#ifdef PX_PLATFORM_LINUX
				m_PanelManager.OpenPopup<ParseDumpPanel>();
#endif
			}
			if (!enabled)
			{
				ImGui::EndDisabled();
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
					ImGui::SetTooltip("PS Vita tools are only available on Linux.");
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
				m_PanelManager.OpenPopup<AboutPanel>();

			ImGui::EndMenu();
		}
	}
}