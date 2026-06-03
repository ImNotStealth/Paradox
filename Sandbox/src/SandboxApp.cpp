#include <Paradox.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    Shared<VertexBuffer> m_VertexBuffer = nullptr;
    Shared<IndexBuffer> m_IndexBuffer = nullptr;
    Camera m_Camera;

	Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
	Shared<UniformBufferSet> m_ColorUBS = UniformBufferSet::Create(sizeof(glm::vec4));

private:
    void Init()
    {
        Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
        shader->SetUniforms({
            { 0, m_CameraUBS, "Camera" },
			{ 1, m_ColorUBS, "Color" }
        });

        PipelineProperties pipelineProps = {};
        pipelineProps.shader = shader;
        pipelineProps.debugName = "Default Pipeline";
        pipelineProps.layout = {
            { VertexBufferDataType::Float2 },
            { VertexBufferDataType::Float3 }
        };
        pipelineProps.cullMode = CullMode::None;

        m_Pipeline = Pipeline::Create(pipelineProps);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);
    }

    void OnUpdate(float deltaTime) override
    {
        m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());
        m_Camera.Update(deltaTime);
        m_CameraUBS->GetCurrent()->SetData(&m_Camera.GetViewProjection(), sizeof(glm::mat4));

        static glm::vec4 testColor = glm::vec4(1.f, 0.f, 1.f, 1.f);
        if (Input::IsKeyPressed(Keyboard::KP0))
            testColor = glm::vec4(1.f, 0.f, 0.f, 1.f);

        if (Input::IsKeyPressed(Keyboard::KP1))
            testColor = glm::vec4(0.f, 1.f, 0.f, 1.f);

        if (Input::IsKeyPressed(Keyboard::KP2))
            testColor = glm::vec4(0.f, 0.f, 1.f, 1.f);

        if (Input::IsKeyPressed(Keyboard::KP3))
            testColor = glm::vec4(1.f, 1.f, 1.f, 1.f);

        m_ColorUBS->GetCurrent()->SetData(&testColor, sizeof(glm::vec4));

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

        Renderer::BeginFrame();
        Renderer::BeginRenderPass(m_Pipeline);

        Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);

        Renderer::EndRenderPass();
        Renderer::EndFrame();

        //TODO: (in order) 
        // Instanced rendering (for quads at least)
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