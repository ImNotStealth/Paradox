#include <Paradox.h>
#include <Paradox/Core/EntryPoint.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef PX_INCLUDE_IMGUI
    #include <imgui.h>
#endif

using namespace Paradox;

class SandboxApp : public Application
{
public:
    SandboxApp(const WindowCreateProperties& windowProps, const CommandLineParser& args)
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
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

	std::vector<uint32_t> m_Indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
    Shared<Pipeline> m_Pipeline = nullptr;
    Shared<VertexBuffer> m_VertexBuffer = nullptr;
    Shared<IndexBuffer> m_IndexBuffer = nullptr;
    Camera m_Camera;

	Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
	Shared<UniformBufferSet> m_ColorUBS = UniformBufferSet::Create(sizeof(glm::vec4));
	glm::vec4 m_TestColor = glm::vec4(1.f, 0.f, 1.f, 1.f);
    Shared<Texture2D> m_TestTexture, m_TextureNiva = nullptr;

    Shared<Framebuffer> m_Framebuffer = nullptr;
    bool m_NeedResize = true;

private:
    void Init()
    {
#ifdef PX_INCLUDE_IMGUI
        ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());
#endif

        m_TestTexture = Texture2D::Create("Test Texture", "textures/texture.jpg");

        TextureProperties nivaProps = {};
        nivaProps.debugName = "Niva";
        nivaProps.magFilter = TextureFilter::Nearest;
        m_TextureNiva = Texture2D::Create(nivaProps, "textures/Controls.png");

        Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
        shader->SetUniformBufferInput(0, m_CameraUBS, "Camera");
        shader->SetUniformBufferInput(1, m_ColorUBS, "Color");
        shader->SetTextureInput(2, m_TestTexture, "TestTexture");
        shader->SetTextureInput(3, m_TextureNiva, "TextureNiva");
        shader->BakeInput();

        FramebufferProperties framebufferProps = {};
        framebufferProps.width = 1280;
        framebufferProps.height = 720;
        framebufferProps.swapchainTarget = true;
        framebufferProps.debugName = "Test Framebuffer";
        m_Framebuffer = Framebuffer::Create(framebufferProps);

        PipelineProperties pipelineProps = {};
        pipelineProps.shader = shader;
        pipelineProps.framebuffer = m_Framebuffer;
        pipelineProps.debugName = "Default Pipeline";
        pipelineProps.layout = {
            { VertexBufferDataType::Float3 },
            { VertexBufferDataType::Float3 },
            { VertexBufferDataType::Float2 }
        };
        pipelineProps.cullMode = CullMode::None;
        m_Pipeline = Pipeline::Create(pipelineProps);

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
            m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());
            m_NeedResize = false;
        }

        //PX_WARN("HELLO");

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

        //TODO: (in order)
        // Instanced rendering (for quads at least)
    }

#ifdef PX_INCLUDE_IMGUI
    void OnImGuiRender(float deltaTime)
    {
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Color", glm::value_ptr(m_TestColor));
        ImGui::End();
    }
#endif
};

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    WindowCreateProperties createProps;
    createProps.title = "Sandbox";
    createProps.width = 1280;
    createProps.height = 720;
    createProps.graphicsAPI = GraphicsAPIType::OpenGL;
    return new SandboxApp(createProps, args);
}