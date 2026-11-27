#include "RuntimeVitaApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

RuntimeVitaApp::RuntimeVitaApp(const RuntimeConfigProperties& runtimeProps, const CommandLineParser& args)
    : Application(runtimeProps.windowProperties, args)
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

    Renderer2D::SetFramebuffer(m_SceneFramebuffer);

    m_Scene = m_AssetManager->GetAsset<Scene>(runtimeProps.startScene);
}

void RuntimeVitaApp::OnUpdate(float deltaTime)
{
    if (Input::IsKeyPressed(Keyboard::KP2))
        PX_WARN("Delta: {0} FPS: {1}", deltaTime, 1.f / deltaTime);

    Renderer2D::BeginFrame();

    m_Scene->Update(deltaTime);

    Renderer2D::EndFrame();

    Renderer::BeginRenderPass(m_PresentPipeline);
    Renderer::DrawIndexed(m_PresentVB, m_PresentIB);
    Renderer::EndRenderPass();
}