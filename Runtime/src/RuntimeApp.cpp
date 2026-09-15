#include <Paradox.h>
#include <Paradox/Core/EntryPoint.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

#ifdef PX_INCLUDE_IMGUI
    #include <imgui.h>
#endif

using namespace Paradox;

class RuntimeApp : public Application
{
public:
    RuntimeApp(const WindowCreateProperties& windowProps, const CommandLineParser& args)
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
    Shared<Framebuffer> m_CompositeFramebuffer = nullptr;
    Shared<Framebuffer> m_SwapchainFramebuffer = nullptr;
    Shared<Texture2D> m_PresentTexture = nullptr;

    Shared<VertexBuffer> m_QuadVB = nullptr;
    Shared<VertexBuffer> m_FullscreenQuadVB = nullptr;
    Shared<IndexBuffer> m_QuadIB = nullptr;
    bool m_NeedResize = true;

    int m_SpiralCount = 0;
    glm::vec3 m_QuadPosition = { 10.f, -10.0f, 0.f };
    float m_TilingFactor = 1.f;
    std::array<Shared<Texture2D>, 15> m_TextureArray;

private:
    void Init()
    {
#ifdef PX_INCLUDE_IMGUI
        ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());
#endif

        m_TestTexture = Texture2D::Create("Test Texture", "Assets/Textures/texture.jpg");

        TextureProperties nivaProps = {};
        nivaProps.debugName = "Niva";
        nivaProps.magFilter = TextureFilter::Nearest;
        m_TextureNiva = Texture2D::Create(nivaProps, "Assets/Textures/Controls.png");

        Shared<Shader> shader = Shader::Create("Default Shader", "shader.vert", "shader.frag");
        shader->SetUniformBufferInput(0, m_CameraUBS, "Camera");
        shader->SetTextureInput(1, m_TestTexture, "TextureArrayTest", 0);
        shader->SetTextureInput(1, m_TextureNiva, "TextureArrayTest", 1);
        shader->BakeInput();

        FramebufferProperties sceneProps = {};
        sceneProps.width = 1280;
        sceneProps.height = 720;
        sceneProps.clear = true;
        sceneProps.swapchainTarget = false;
        sceneProps.attachments = { { ImageFormat::RGBA }, { ImageFormat::Depth32F } };
        sceneProps.debugName = "Scene Framebuffer";
        m_SceneFramebuffer = Framebuffer::Create(sceneProps);

        sceneProps.clear = false;
        sceneProps.debugName = "Composite";
        sceneProps.attachments = {
            { ImageFormat::RGBA,     m_SceneFramebuffer->GetAttachmentImage(0) },
            { ImageFormat::Depth32F, m_SceneFramebuffer->GetAttachmentImage(1) }
        };
        m_CompositeFramebuffer = Framebuffer::Create(sceneProps);

        FramebufferProperties swapProps = {};
        swapProps.width = 1280;
        swapProps.height = 720;
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
        m_PresentTexture = Texture2D::CreateFromImage(presentTexProps, m_CompositeFramebuffer->GetAttachmentImage(0));
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

        std::vector<QuadVertex> fullScreenQuadVerts = {
            {{ 0.f, -1.f}, {0.f, 0.f}},
            {{ 0.f, -1.f}, {1.f, 0.f}},
            {{ 1.f,  1.f}, {1.f, 1.f}},
            {{-1.f,  1.f}, {0.f, 1.f}},
        };
        std::vector<uint32_t> quadIndices = { 0, 1, 2, 2, 3, 0 };
        m_QuadVB = VertexBuffer::Create(quadVerts.data(), (uint32_t)(sizeof(QuadVertex) * quadVerts.size()), VertexBufferUsage::Static);
        m_FullscreenQuadVB = VertexBuffer::Create(fullScreenQuadVerts.data(), (uint32_t)(sizeof(QuadVertex) * fullScreenQuadVerts.size()), VertexBufferUsage::Static);
        m_QuadIB = IndexBuffer::Create(quadIndices.data(), (uint32_t)quadIndices.size(), IndexBufferUsage::Static);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

        Renderer2D::SetFramebuffer(m_CompositeFramebuffer);

        TextureProperties arrayTextureProps = {};
		arrayTextureProps.magFilter = TextureFilter::Nearest;
		arrayTextureProps.minFilter = TextureFilter::Nearest;
        for (int i = 0; i < 15; i++)
        {
            arrayTextureProps.debugName = "Texture Array" + std::to_string(i);
            std::string texturePath = "Assets/Textures/texture" + std::to_string(i) + ".png";
            m_TextureArray[i] = Texture2D::Create(arrayTextureProps, texturePath);
		}
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
            m_SceneFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_CompositeFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_SwapchainFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());

            m_NeedResize = false;
        }

        m_Camera.Update(deltaTime);
        m_CameraUBS->GetCurrent()->SetData(&m_Camera.GetViewProjection(), sizeof(glm::mat4));

        Renderer::BeginRenderPass(m_ScenePipeline);
        Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
        Renderer::EndRenderPass();

		Renderer2D::Begin(m_Camera.GetViewProjection());

        const unsigned int numPhi = m_SpiralCount + 1;
        const unsigned int numTheta = numPhi / 2;

        const float phiStep = glm::two_pi<float>() / static_cast<float>(numPhi);
        const float thetaStep = glm::pi<float>() / static_cast<float>(numTheta);
        const glm::vec3 quadScale = glm::vec3(0.2f, 0.2f, 1.0f);

        for (unsigned int i = 0; i < numPhi; i++) {
            for (unsigned int j = 0; j <= numTheta; j++) {
                float phi = i * phiStep;
                float theta = j * thetaStep;

                glm::vec3 v1 = { sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi) };
                glm::vec3 normal = glm::normalize(v1);

                glm::mat4 mat = glm::mat4(1.0f);
                //mat = glm::translate(glm::mat4(1.0f), v1);
                mat = glm::translate(glm::mat4(1.0f), { v1.x - 0.5f, v1.y - 0.5f, v1.z - 0.5f });

                glm::quat orientation = glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), normal);
                mat *= glm::toMat4(orientation);

                mat = glm::scale(mat, quadScale);
                mat = glm::translate(mat, glm::vec3(-0.5f, -0.5f, 0.0f));

                Renderer2D::DrawQuad(mat, m_TextureArray[i % 15], glm::vec4(1.f), m_TilingFactor);
            }
        }
        Renderer2D::End();

		glm::mat4 orthoCam = glm::ortho(0.f, (float)GetWindow().GetWidth(), 0.f, (float)GetWindow().GetHeight());
		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
            orthoCam[1][1] *= -1.f;
		Renderer2D::Begin(orthoCam);
        Renderer2D::DrawQuad(m_QuadPosition, {50.f, 50.f}, {1.f, 1.f, 1.f, 1.f});
        Renderer2D::End();
        
        Renderer::BeginRenderPass(m_PresentPipeline);
        Renderer::DrawIndexed(m_QuadVB, m_QuadIB);
        Renderer::EndRenderPass();
    }

#ifdef PX_INCLUDE_IMGUI
    void OnImGuiRender(float deltaTime) override
    {
		ImGui::Begin("Settings");
        ImGui::DragFloat3("Position", glm::value_ptr(m_Camera.GetPosition()));
        ImGui::DragFloat3("QuadPos", glm::value_ptr(m_QuadPosition));
        ImGui::DragInt("Spiral Count", &m_SpiralCount, 1, 0, 10000);
        ImGui::SliderFloat("Tiling Factor", &m_TilingFactor, 0.f, 5.f);
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
};

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    WindowCreateProperties createProps;
    createProps.title = "Runtime";
    createProps.width = 1280;
    createProps.height = 720;
    createProps.graphicsAPI = GraphicsAPIType::Vulkan;
    return new RuntimeApp(createProps, args);
}