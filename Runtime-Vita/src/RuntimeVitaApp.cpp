#include <Paradox.h>
#include <Paradox/Core/EntryPoint.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Paradox;

class RuntimeVitaApp : public Application
{
public:
    RuntimeVitaApp(const WindowCreateProperties& windowProps, const CommandLineParser& args)
        : Application(windowProps, args)
    {
        Init();
    }

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
    Shared<VertexBuffer> m_VertexBuffer = nullptr;
    Shared<IndexBuffer> m_IndexBuffer = nullptr;
    Camera m_Camera;

	Shared<UniformBufferSet> m_CameraUBS = UniformBufferSet::Create(sizeof(glm::mat4));
    Shared<Texture2D> m_TestTexture, m_TextureNiva = nullptr;

    Shared<Pipeline> m_ScenePipeline = nullptr;
    Shared<Pipeline> m_PresentPipeline = nullptr;
    Shared<Framebuffer> m_SceneFramebuffer = nullptr;
    Shared<Framebuffer> m_SwapchainFramebuffer = nullptr;
    Shared<Texture2D> m_PresentTexture = nullptr;

    Shared<VertexBuffer> m_QuadVB = nullptr;
    Shared<IndexBuffer> m_QuadIB = nullptr;
    int m_SpiralCount = 2;

    Scene m_Scene;

private:
    void Init()
    {
        m_TestTexture = Texture2D::Create("Test Texture", "Assets/Textures/texture.jpg");

        TextureProperties nivaProps = {};
        nivaProps.debugName = "Niva";
        nivaProps.magFilter = TextureFilter::Nearest;
        m_TextureNiva = Texture2D::Create(nivaProps, "Assets/Textures/Controls.png");

        Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
        shader->SetUniformBufferInput(0, m_CameraUBS, "Camera");
        shader->SetTextureInput(1, m_TestTexture, "TestTexture");
        shader->SetTextureInput(2, m_TextureNiva, "TextureNiva");
        shader->BakeInput();

        FramebufferProperties sceneProps = {};
        sceneProps.width = 960;
        sceneProps.height = 544;
        sceneProps.clear = true;
        sceneProps.swapchainTarget = false;
        sceneProps.attachments = { { ImageFormat::RGBA }, { ImageFormat::Depth32F } };
        sceneProps.debugName = "Scene Framebuffer";
        m_SceneFramebuffer = Framebuffer::Create(sceneProps);

        FramebufferProperties swapProps = {};
        swapProps.width = 960;
        swapProps.height = 544;
        swapProps.swapchainTarget = true;
        swapProps.attachments = { { ImageFormat::RGBA } };
        swapProps.debugName = "Swapchain Framebuffer";
        m_SwapchainFramebuffer = Framebuffer::Create(swapProps);

        PipelineProperties scenePipelineProps = {};
        scenePipelineProps.shader = shader;
        scenePipelineProps.framebuffer = m_SceneFramebuffer;
        scenePipelineProps.debugName = "Scene Pipeline";
        scenePipelineProps.layout = { { VertexBufferDataType::Float3 }, { VertexBufferDataType::Float2 } };
        scenePipelineProps.cullMode = CullMode::None;
        m_ScenePipeline = Pipeline::Create(scenePipelineProps);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

        Shared<Shader> presentShader = Shader::Create("Present Shader", "presentShader.vert", "presentShader.frag");
        TextureProperties presentTexProps = {};
        presentTexProps.debugName = "Present Texture";
        m_PresentTexture = Texture2D::CreateFromImage(presentTexProps, m_SceneFramebuffer->GetAttachmentImage(0));
        presentShader->SetTextureInput(0, m_PresentTexture, "PresentTexture");
        presentShader->BakeInput();

        PipelineProperties presentPipelineProps = {};
        presentPipelineProps.shader = presentShader;
        presentPipelineProps.framebuffer = m_SwapchainFramebuffer;
        presentPipelineProps.debugName = "Present Pipeline";
        presentPipelineProps.layout = { { VertexBufferDataType::Float2 }, { VertexBufferDataType::Float2 } };
        presentPipelineProps.cullMode = CullMode::None;
        m_PresentPipeline = Pipeline::Create(presentPipelineProps);

        struct QuadVertex { glm::vec2 pos; glm::vec2 uv; };
        std::vector<QuadVertex> quadVerts = {
            {{-1.f, -1.f}, {0.f, 0.f}}, {{ 1.f, -1.f}, {1.f, 0.f}},
            {{ 1.f,  1.f}, {1.f, 1.f}}, {{-1.f,  1.f}, {0.f, 1.f}},
        };
        std::vector<uint32_t> quadIndices = { 0, 1, 2, 2, 3, 0 };
        m_QuadVB = VertexBuffer::Create(quadVerts.data(), (uint32_t)(sizeof(QuadVertex) * quadVerts.size()), VertexBufferUsage::Static);
        m_QuadIB = IndexBuffer::Create(quadIndices.data(), (uint32_t)quadIndices.size(), IndexBufferUsage::Static);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

        m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());
        Renderer2D::SetFramebuffer(m_SceneFramebuffer);

        Entity entity1 = m_Scene.CreateEntity("Test");
        entity1.GetComponent<TransformComponent>().position = { 0.f, 2.f, 0.f };
        entity1.GetComponent<TransformComponent>().scale = { 4.f, 0.5f, 1.0f };
        entity1.AddComponent<SpriteComponent>();

        Entity entity2 = m_Scene.CreateEntity("Test2");
        entity2.GetComponent<TransformComponent>().position = { 1.f, 1.f, 0.f };
        entity2.AddComponent<SpriteComponent>();
    }

    void OnUpdate(float deltaTime) override
    {
        m_Camera.Update(deltaTime);
        m_CameraUBS->GetCurrent()->SetData(&m_Camera.GetViewProjection(), sizeof(glm::mat4));

        Renderer::BeginRenderPass(m_ScenePipeline);
        Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
        Renderer::EndRenderPass();

        if (Input::IsKeyPressed(Keyboard::KP0))
            m_SpiralCount = std::max(0, m_SpiralCount - 50);
        else if (Input::IsKeyPressed(Keyboard::KP1))
            m_SpiralCount = std::min(3000, m_SpiralCount + 50);
        else if (Input::IsKeyPressed(Keyboard::KP2))
            PX_WARN("Delta: {0} FPS: {1} SpiralCount: {2}", deltaTime, 1.f / deltaTime, m_SpiralCount);

        Renderer2D::Begin(m_Camera.GetViewProjection());

        const float anglePerCount = glm::radians(15.f);
        const float startRadius = 5.f;
        const float falloff = 0.001f;

        for (int i = 0; i < m_SpiralCount; i++)
        {
            const float radius = startRadius / (1.f + i * falloff);
            float x = radius * cos(i * anglePerCount);
            float y = radius * sin(i * anglePerCount);

            auto hash = [](uint32_t n) -> uint32_t {
                n = (n ^ 61u) ^ (n >> 16);
                n *= 9u;
                n = n ^ (n >> 4);
                n *= 0x27d4eb2du;
                n = n ^ (n >> 15);
                return n;
            };

            uint32_t h = hash((uint32_t)i);
            float r = ((h >> 0)  & 0xFF) / 255.f;
            float g = ((h >> 8)  & 0xFF) / 255.f;
            float b = ((h >> 16) & 0xFF) / 255.f;

            Renderer2D::DrawQuad(glm::translate(glm::mat4(1.f), { x, y, -((float)i * 0.01f) }), {r, g, b, 1.f});
        }
        Renderer2D::End();

        m_Scene.Update(m_Camera.GetViewProjection(), deltaTime);

        Renderer::BeginRenderPass(m_PresentPipeline);
        Renderer::DrawIndexed(m_QuadVB, m_QuadIB);
        Renderer::EndRenderPass();
    }
};

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    WindowCreateProperties createProps;
    createProps.title = "Runtime-Vita";
    createProps.graphicsAPI = GraphicsAPIType::OpenGL;
    return new RuntimeVitaApp(createProps, args);
}