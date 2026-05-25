#include <Paradox.h>

#define GLM_FORCE_RADIANS
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
    Camera m_Camera = Camera(800 / 600, 1);

    struct CameraData
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
	Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(CameraData));

private:
    void Init()
    {
        Shared<Shader> shader = Shader::Create("Default Shader", "shaders/compiled/shader.vert.spv", "shaders/compiled/shader.frag.spv");
        shader->SetUniform(m_CameraUBS);

        PipelineProperties pipelineProps = {};
        pipelineProps.shader = shader;
        pipelineProps.debugName = "Default Pipeline";
        pipelineProps.layout = {
            { VertexBufferDataType::Float2 },
            { VertexBufferDataType::Float3 }
        };
        pipelineProps.cullMode = CullMode::Back;
        pipelineProps.wireframe = true;
        pipelineProps.wireframeWidth = 10.f;

        m_Pipeline = Pipeline::Create(pipelineProps);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Dynamic);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Dynamic);
    }

    void OnUpdate() override
    {
        m_Camera.Update();
        CameraData ubo{};
        static float f = 0.f;
		f += 0.0001f;
        ubo.model = glm::rotate(glm::mat4(1.0f), f * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), GetWindow().GetWidth() / (float)GetWindow().GetHeight(), 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        m_CameraUBS->GetCurrent()->SetData(&ubo, sizeof(CameraData));

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
        // Camera (world space coordinates)
        // Look into how to exclude unnecessary libs for platforms (ex: remove Vulkan and spdlog for PSVita)
        // Instanced rendering (for quads at least)
    }
};

Application* Paradox::CreateApplication()
{
    WindowCreateProperties createProps;
    createProps.title = "Sandbox";
    createProps.width = 800;
    createProps.height = 600;
    return new SandboxApp(createProps);
}