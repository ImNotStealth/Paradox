#include "RuntimeVitaApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

RuntimeVitaApp::RuntimeVitaApp(const WindowCreateProperties& windowProps, const CommandLineParser& args)
    : Application(windowProps, args)
{
    const uint32_t width = GetWindow().GetWidth();
    const uint32_t height = GetWindow().GetHeight();

    m_AssetManager = CreateShared<AssetManager>("Assets");
    AssetManager::SetInstance(m_AssetManager.get());

    FramebufferProperties sceneProps = {};
    sceneProps.width = width;
    sceneProps.height = height;
    sceneProps.clear = true;
    sceneProps.swapchainTarget = false;
    sceneProps.attachments = { { ImageFormat::RGBA }, { ImageFormat::Depth32F } };
    sceneProps.debugName = "Scene Framebuffer";
    m_SceneFramebuffer = Framebuffer::Create(sceneProps);

    FramebufferProperties swapProps = {};
    swapProps.width = width;
    swapProps.height = height;
    swapProps.swapchainTarget = true;
    swapProps.attachments = { { ImageFormat::RGBA } };
    swapProps.debugName = "Swapchain Framebuffer";
    m_SwapchainFramebuffer = Framebuffer::Create(swapProps);

    Shared<Shader> presentShader = Shader::Create("Present", "Present.vert", "Present.frag");
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
    std::vector<QuadVertex> presentVerts = {
        {{-1.f, -1.f}, {0.f, 0.f}}, {{ 1.f, -1.f}, {1.f, 0.f}},
        {{ 1.f,  1.f}, {1.f, 1.f}}, {{-1.f,  1.f}, {0.f, 1.f}},
    };
    std::vector<uint32_t> presentIndices = { 0, 1, 2, 2, 3, 0 };
    m_PresentVB = VertexBuffer::Create(presentVerts.data(), (uint32_t)(sizeof(QuadVertex) * presentVerts.size()), VertexBufferUsage::Static);
    m_PresentIB = IndexBuffer::Create(presentIndices.data(), (uint32_t)presentIndices.size(), IndexBufferUsage::Static);

    m_Camera.SetViewportSize((float)width, (float)height);
    Renderer2D::SetFramebuffer(m_SceneFramebuffer);

    m_Scene = m_AssetManager->GetAsset<Scene>(UUID("00bfd13e48ba4e24920dc04f53ef1208"));
}

void RuntimeVitaApp::OnUpdate(float deltaTime)
{
    m_Camera.Update(deltaTime);

    if (Input::IsKeyPressed(Keyboard::KP0))
        m_SpiralCount = std::max(0, m_SpiralCount - 50);
    else if (Input::IsKeyPressed(Keyboard::KP1))
        m_SpiralCount = std::min(3000, m_SpiralCount + 50);
    else if (Input::IsKeyPressed(Keyboard::KP2))
        PX_WARN("Delta: {0} FPS: {1} SpiralCount: {2}", deltaTime, 1.f / deltaTime, m_SpiralCount);

    Renderer2D::BeginFrame();
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

    m_Scene->Update(m_Camera.GetViewProjection(), deltaTime);

    Renderer2D::EndFrame();

    Renderer::BeginRenderPass(m_PresentPipeline);
    Renderer::DrawIndexed(m_PresentVB, m_PresentIB);
    Renderer::EndRenderPass();
}