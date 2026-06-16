#include "pxpch.h"
#include "EditorApp.h"

#include <Paradox/ImGui/ImGuiUtils.h>

namespace Paradox
{
	void EditorApp::Init()
	{
		ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());

		// Editor
		m_ConsoleLogPanel = CreateUnique<ConsoleLogPanel>();

		Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
		shader->SetUniforms({
			{ 0, m_CameraUBS, "Camera" },
			{ 1, m_ColorUBS, "Color" }
		});

		FramebufferProperties framebufferProps = {};
		framebufferProps.width = 1280;
		framebufferProps.height = 720;
		framebufferProps.swapchainTarget = false;
		framebufferProps.debugName = "Viewport Framebuffer";
		m_Framebuffer = Framebuffer::Create(framebufferProps);

		PipelineProperties pipelineProps = {};
		pipelineProps.shader = shader;
		pipelineProps.framebuffer = m_Framebuffer;
		pipelineProps.debugName = "Default Pipeline";
		pipelineProps.layout = {
			{ VertexBufferDataType::Float2 },
			{ VertexBufferDataType::Float3 }
		};
		pipelineProps.cullMode = CullMode::None;
		m_Pipeline = Pipeline::Create(pipelineProps);

		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

		PX_INFO("Hello!");

		// Maximize the window here instead of in CreateApplication to let the renderer start up (to prevent the white fullscreen)
		GetWindow().Maximize();
	}

	void EditorApp::OnEvent(Event& event)
	{
		if (m_ConsoleLogPanel)
			m_ConsoleLogPanel->OnEvent(event);

		if (event.GetEventType() == EventType::WindowResize)
			m_NeedResize = true;
	}

	void EditorApp::OnUpdate(float deltaTime)
	{
		const FramebufferProperties& fbProps = m_Framebuffer->GetProperties();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (fbProps.width != m_ViewportSize.x || fbProps.height != m_ViewportSize.y))
		{
			m_Framebuffer->OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_NeedResize = false;
		}

		m_Camera.Update(deltaTime);
		m_CameraUBS->GetCurrent()->SetData(&m_Camera.GetViewProjection(), sizeof(glm::mat4));

		if (Input::IsKeyPressed(Keyboard::Z))
			m_TestColor = glm::vec4(1.f, 0.f, 0.f, 1.f);

		if (Input::IsKeyPressed(Keyboard::X))
			m_TestColor = glm::vec4(0.f, 1.f, 0.f, 1.f);

		if (Input::IsKeyPressed(Keyboard::C))
			m_TestColor = glm::vec4(0.f, 0.f, 1.f, 1.f);

		if (Input::IsKeyPressed(Keyboard::V))
			m_TestColor = glm::vec4(1.f, 1.f, 1.f, 1.f);

		m_ColorUBS->GetCurrent()->SetData(&m_TestColor, sizeof(glm::vec4));

		//if (m_Vertices[0].pos.x > 0.5f)
		//	m_Vertices[0].pos.x = -0.5f;
		//m_Vertices[0].pos.x += 0.00005f;
		//m_VertexBuffer->SetData(m_Vertices.data(), (uint32_t)(sizeof(Vertex) * m_Vertices.size()));
		//
		//if (m_Vertices[0].pos.x > -0.4f)
		//{
		//    m_Indices = { 0, 1, 2, 0, 1, 3 };
		//    m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size());
		//}

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
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					Application::Get().Stop();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetImageID();
		ImGuiUtils::Image(textureID, viewportPanelSize);
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Color", glm::value_ptr(m_TestColor));
		ImGui::Spacing();

		ImGui::Text("Graphics API: %s", GraphicsContext::GraphicsAPIToString(GraphicsContext::GetGraphicsAPI()).c_str());
		ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::Text("VSync: %s", GetWindow().IsVSync() ? "On" : "Off");
		if (ImGui::Button("Toggle VSync"))
			GetWindow().SetVSync(!GetWindow().IsVSync());

		ImGui::End();

		if (m_ConsoleLogPanel)
			m_ConsoleLogPanel->OnImGuiRender();

		ImGui::End(); //Dockspace

		ImGui::ShowDemoWindow();
	}
}