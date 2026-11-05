#include "RuntimeApp.h"

#include <glm/gtc/matrix_transform.hpp>

#ifdef PX_INCLUDE_IMGUI
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#endif

RuntimeApp::RuntimeApp(const RuntimeConfigProperties& runtimeProps, const CommandLineParser& args)
    : Application(runtimeProps.windowProperties, args)
{
#ifdef PX_INCLUDE_IMGUI
    ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());
#endif

    m_AssetManager = CreateShared<AssetManager>("Assets");
    AssetManager::SetInstance(m_AssetManager.get());

    FramebufferProperties sceneProps = {};
    sceneProps.width = 1280;
    sceneProps.height = 720;
    sceneProps.clear = true;
    sceneProps.swapchainTarget = false;
    sceneProps.attachments = { { ImageFormat::RGBA },{ ImageFormat::Depth32F } };
    sceneProps.debugName = "Scene Framebuffer";
    m_SceneFramebuffer = Framebuffer::Create(sceneProps);

    FramebufferProperties swapProps = {};
    swapProps.width = 1280;
    swapProps.height = 720;
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
    presentPipelineProps.layout = { { VertexBufferDataType::Float2 },{ VertexBufferDataType::Float2 } };
    presentPipelineProps.cullMode = CullMode::None;
    m_PresentPipeline = Pipeline::Create(presentPipelineProps);

    struct QuadVertex { glm::vec2 pos; glm::vec2 uv; };
    std::vector<QuadVertex> presentVerts = {
        { { -1.f, -1.f },{ 0.f, 0.f } },{ { 1.f, -1.f },{ 1.f, 0.f } },
        { { 1.f,  1.f },{ 1.f, 1.f } },{ { -1.f,  1.f },{ 0.f, 1.f } },
    };
    std::vector<uint32_t> presentIndices = { 0, 1, 2, 2, 3, 0 };
    m_PresentVB = VertexBuffer::Create(presentVerts.data(), (uint32_t)(sizeof(QuadVertex) * presentVerts.size()), VertexBufferUsage::Static);
    m_PresentIB = IndexBuffer::Create(presentIndices.data(), (uint32_t)presentIndices.size(), IndexBufferUsage::Static);

    Renderer2D::SetFramebuffer(m_SceneFramebuffer);

    m_Scene = m_AssetManager->GetAsset<Scene>(runtimeProps.startScene);
}

void RuntimeApp::OnEvent(Event & event)
{
    if (event.GetEventType() == EventType::WindowResize)
        m_NeedResize = true;
}

void RuntimeApp::OnUpdate(float deltaTime)
{
    if (m_NeedResize && !IsMinimized())
    {
        m_SceneFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
        m_SwapchainFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
        m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());

        m_NeedResize = false;
    }

    m_Camera.Update(deltaTime);

    Renderer2D::BeginFrame();

    m_Scene->Update(m_Camera.GetViewProjection(), deltaTime);

    glm::mat4 orthoCam = glm::ortho(0.f, (float)GetWindow().GetWidth(), 0.f, (float)GetWindow().GetHeight());
    if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
        orthoCam[1][1] *= -1.f;
    Renderer2D::Begin(orthoCam);
    Renderer2D::DrawQuad(m_QuadPosition, { 50.f, 50.f }, { 1.f, 1.f, 1.f, 1.f });
    Renderer2D::End();

    Renderer2D::EndFrame();

    Renderer::BeginRenderPass(m_PresentPipeline);
    Renderer::DrawIndexed(m_PresentVB, m_PresentIB);
    Renderer::EndRenderPass();
}

#ifdef PX_INCLUDE_IMGUI
void RuntimeApp::OnImGuiRender(float deltaTime)
{
    ImGui::Begin("Settings");
    ImGui::DragFloat3("Position", glm::value_ptr(m_Camera.GetPosition()));
    ImGui::DragFloat3("QuadPos", glm::value_ptr(m_QuadPosition));
    ImGui::Text("Renderer2D");
    ImGui::Text("\tDrawCalls: %d", Renderer2D::GetStatistics().drawCalls);
    ImGui::Text("\tQuadCount: %d", Renderer2D::GetStatistics().quadCount);
    ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    bool isVsync = GetWindow().IsVSync();
    if (ImGui::Checkbox("Toggle VSync", &isVsync))
        GetWindow().SetVSync(isVsync);
    ImGui::End();
}
#endif