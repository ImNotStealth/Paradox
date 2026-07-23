#include "pxpch.h"
#include "EditorApp.h"

#include "Panels/ConsoleLogPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/CreateProjectPanel.h"
#include "Panels/AboutPanel.h"
#ifdef PX_PLATFORM_LINUX
#include "Platform/Linux/ParseDumpPanel.h"
#endif

#include <Paradox/Core/FileSystem.h>
#include <Paradox/ImGui/ImGuiUtils.h>

namespace Paradox
{
	void EditorApp::Init()
	{
		ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());

		//Editor
		m_PanelManager.RegisterPanel<ConsoleLogPanel>(true);
		m_PanelManager.RegisterPanel<AssetBrowserPanel>(true);

		m_Texture = Texture2D::Create("Test Texture", "Assets/Textures/texture.jpg");

		TextureProperties nivaProps = {};
		nivaProps.debugName = "Niva";
		nivaProps.magFilter = TextureFilter::Nearest;
		m_TextureNiva = Texture2D::Create(nivaProps, "Assets/Textures/Controls.png");

		Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
		shader->SetUniformBufferInput(0, m_CameraUBS, "Camera");
		shader->SetTextureInput(1, m_Texture, "Texture");
		shader->SetTextureInput(2, m_TextureNiva, "TextureNiva");
		shader->BakeInput();

		FramebufferProperties framebufferProps = {};
		framebufferProps.width = 1280;
		framebufferProps.height = 720;
		framebufferProps.swapchainTarget = false;
		framebufferProps.attachments = { { ImageFormat::RGBA }, { ImageFormat::Depth32F } };
		framebufferProps.debugName = "Viewport Framebuffer";
		m_Framebuffer = Framebuffer::Create(framebufferProps);

		PipelineProperties pipelineProps = {};
		pipelineProps.shader = shader;
		pipelineProps.framebuffer = m_Framebuffer;
		pipelineProps.debugName = "Default Pipeline";
		pipelineProps.layout = {
			{ VertexBufferDataType::Float3 },
			{ VertexBufferDataType::Float2 }
		};
		pipelineProps.cullMode = CullMode::None;
		m_Pipeline = Pipeline::Create(pipelineProps);

		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

		if (GetCommandLineArgs().HasFlag("project"))
		{
			std::string projectPath = GetCommandLineArgs().GetRequired<std::string>("project");
			Project::SetActive(Project(projectPath));
		}
		else
			Project::SetActive(Project());

		// Maximize the window here instead of in CreateApplication to let the renderer start up (to prevent the white fullscreen)
		GetWindow().Maximize();
	}

	void EditorApp::OnEvent(Event& event)
	{
		m_PanelManager.OnEvent(event);

		if (event.GetEventType() == EventType::WindowResize)
			m_NeedResize = true;
	}

	void EditorApp::OnUpdate(float deltaTime)
	{
		const FramebufferProperties& fbProps = m_Framebuffer->GetProperties();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (fbProps.width != m_ViewportSize.x || fbProps.height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_NeedResize = false;
		}

		m_Camera.Update(deltaTime);
		m_CameraUBS->GetCurrent()->SetData(&m_Camera.GetViewProjection(), sizeof(glm::mat4));

		Renderer::BeginRenderPass(m_Pipeline);
		Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
		Renderer::EndRenderPass();
	}

	void EditorApp::OnImGuiRender(float deltaTime)
	{
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		
		ImVec2 uv0, uv1;
		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::OpenGL)
		{
			uv0 = ImVec2(0, 1);
			uv1 = ImVec2(1, 0);
		}
		else
		{
			uv0 = ImVec2(0, 0);
			uv1 = ImVec2(1, 1);
		}

		ImGuiUtils::Image(m_Framebuffer->GetAttachmentImage(0), viewportPanelSize, uv0, uv1);
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Settings");
		ImGui::Text("Graphics API: %s", GraphicsContext::GraphicsAPIToString(GraphicsContext::GetGraphicsAPI()).c_str());
		ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Viewport Size: %.0f x %.0f", m_ViewportSize.x, m_ViewportSize.y);
		
		bool isVsync = GetWindow().IsVSync();
		if (ImGui::Checkbox("Toggle VSync", &isVsync))
			GetWindow().SetVSync(isVsync);

		ImGui::End();

		m_PanelManager.OnImGuiRender();

		ImGui::End(); //Dockspace

		ImGui::ShowDemoWindow();
	}

	void EditorApp::RenderMenuBar()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Create new Project..."))
				m_PanelManager.OpenPopup<CreateProjectPanel>();

			if (ImGui::MenuItem("Open Project..."))
			{
				std::filesystem::path path = FileSystem::SelectFile("Select Project file", "Paradox Project|*.px");
				if (!path.empty())
					Project::SetActive(Project(path));
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