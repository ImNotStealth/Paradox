#include <Paradox.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Paradox/Renderer/Framebuffer.h>

#include <imgui.h>

using namespace Paradox;

class SandboxApp : public Application
{
public:
    SandboxApp(const WindowCreateProperties& windowProps)
        : Application(windowProps)
    {
        Init();
    }

private:
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;
    };

    std::vector<Vertex> m_Vertices = {
        {{-0.5f, -0.5f}, {1.f, 0.f, 0.f}},
        {{ 0.5f, -0.5f}, {0.f, 1.f, 0.f}},
        {{ 0.5f,  0.5f}, {0.f, 0.f, 1.f}},
        {{-0.5f,  0.5f}, {1.f, 1.f, 1.f}}
    };

    std::vector<uint32_t> m_Indices = { 0, 1, 2, 2, 3, 0 };
    Shared<Pipeline> m_Pipeline = nullptr;
    Shared<Pipeline> m_Pipeline2 = nullptr;
    Shared<VertexBuffer> m_VertexBuffer = nullptr;
    Shared<IndexBuffer> m_IndexBuffer = nullptr;
    Camera m_Camera;

	Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
	Shared<UniformBufferSet> m_ColorUBS = UniformBufferSet::Create(sizeof(glm::vec4));
	glm::vec4 m_TestColor = glm::vec4(1.f, 0.f, 1.f, 1.f);

    Shared<Framebuffer> m_Framebuffer = nullptr;
    Shared<Framebuffer> m_FramebufferSwapchain = nullptr;
    bool m_NeedResize = true;

private:
    void Init()
    {
        ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());

        Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
        shader->SetUniforms({
            { 0, m_CameraUBS, "Camera" },
			{ 1, m_ColorUBS, "Color" }
        });

        Shared<Shader> shader2 = Shader::Create("Default Shader", "shader.vert", "shader.frag");
        shader2->SetUniforms({
            { 0, m_CameraUBS, "Camera" },
            { 1, m_ColorUBS, "Color" }
        });

        FramebufferProperties framebufferProps = {};
        framebufferProps.width = 1280;
        framebufferProps.height = 720;
        framebufferProps.swapchainTarget = false;
        framebufferProps.clearColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.f);
        framebufferProps.debugName = "Test Framebuffer";
        m_Framebuffer = Framebuffer::Create(framebufferProps);

        FramebufferProperties framebufferProps2 = {};
        framebufferProps2.width = 1280;
        framebufferProps2.height = 720;
        framebufferProps2.swapchainTarget = true;
        framebufferProps2.clearColor = glm::vec4(0.f, 0.7f, 0.f, 1.f);
        framebufferProps2.debugName = "Second Framebuffer";
        m_FramebufferSwapchain = Framebuffer::Create(framebufferProps2);

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

        PipelineProperties pipelineProps2 = {};
        pipelineProps2.shader = shader2;
        pipelineProps2.framebuffer = m_FramebufferSwapchain;
        pipelineProps2.debugName = "Swapchain Pipeline";
        pipelineProps2.layout = {
            { VertexBufferDataType::Float2 },
            { VertexBufferDataType::Float3 }
        };
        pipelineProps2.cullMode = CullMode::None;
        m_Pipeline2 = Pipeline::Create(pipelineProps2);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);
    }

    void OnEvent(Event& event) override
    {
        if (event.GetEventType() == EventType::WindowResize)
            m_NeedResize = true;
    }

    void OnUpdate(float deltaTime) override
    {
        if (m_NeedResize && !IsMinimized())
        {
            m_Framebuffer->OnResize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_FramebufferSwapchain->OnResize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());
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

        Renderer::BeginRenderPass(m_Pipeline2);
        Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
        Renderer::EndRenderPass();

        //TODO: (in order)
        // Fix minimizing freezing entire app
        // Instanced rendering (for quads at least)
    }

    void OnImGuiRender(float deltaTime)
    {
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Color", glm::value_ptr(m_TestColor));
        ImGui::Text("m_Framebuffer Image ID: %d", m_Framebuffer->GetImageID());
		ImGui::Image(m_Framebuffer->GetImageID(), ImVec2((float)m_Framebuffer->GetProperties().width, (float)m_Framebuffer->GetProperties().height), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
    }
};

Application* Paradox::CreateApplication()
{
    WindowCreateProperties createProps;
    createProps.title = "Sandbox";
    createProps.width = 1280;
    createProps.height = 720;
    return new SandboxApp(createProps);
}