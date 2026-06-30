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

    Shared<Pipeline> m_ScenePipeline = nullptr;
    Shared<Pipeline> m_TestPipeline = nullptr;
    Shared<Pipeline> m_PresentPipeline = nullptr;
    Shared<Shader> m_PresentShader = nullptr;
    Shared<Framebuffer> m_SceneFramebuffer = nullptr;   // offscreen, color+depth
    Shared<Framebuffer> m_TestFramebuffer = nullptr;   // offscreen, color+depth
    Shared<Framebuffer> m_SwapchainFramebuffer = nullptr; // color only
    Shared<Texture2D> m_PresentTexture = nullptr;

    Shared<VertexBuffer> m_QuadVB = nullptr;
    Shared<VertexBuffer> m_SecondQuadVB = nullptr;
    Shared<IndexBuffer> m_QuadIB = nullptr;
    bool m_NeedResize = false;

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

        FramebufferProperties sceneProps = {};
        sceneProps.width = 1280;
        sceneProps.height = 720;
        sceneProps.clear = true;
        sceneProps.swapchainTarget = false;
        sceneProps.attachments = { ImageFormat::RGBA, ImageFormat::Depth32F };
        sceneProps.debugName = "Scene Framebuffer";
        m_SceneFramebuffer = Framebuffer::Create(sceneProps);

        sceneProps.clear = false;
        sceneProps.debugName = "TestFramebuffer";
        sceneProps.clearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
        sceneProps.attachments = { ImageFormat::RGBA };
        sceneProps.attachmentImages = { m_SceneFramebuffer->GetAttachmentImage(0) };
        m_TestFramebuffer = Framebuffer::Create(sceneProps);

        FramebufferProperties swapProps = {};
        swapProps.width = 1280;
        swapProps.height = 720;
        swapProps.swapchainTarget = true;
        swapProps.attachments = { ImageFormat::RGBA }; // no depth
        swapProps.debugName = "Swapchain Framebuffer";
        m_SwapchainFramebuffer = Framebuffer::Create(swapProps);

        PipelineProperties scenePipelineProps = {};
        scenePipelineProps.shader = shader;
        scenePipelineProps.framebuffer = m_SceneFramebuffer;
        scenePipelineProps.debugName = "Scene Pipeline";
        scenePipelineProps.layout = { { VertexBufferDataType::Float3 }, { VertexBufferDataType::Float3 }, { VertexBufferDataType::Float2 } };
        scenePipelineProps.cullMode = CullMode::None;
        m_ScenePipeline = Pipeline::Create(scenePipelineProps);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);

        // Present pass
        m_PresentShader = Shader::Create("Present Shader", "presentShader.vert", "presentShader.frag");
        TextureProperties presentTexProps = {};
        presentTexProps.debugName = "Present Texture";
        m_PresentTexture = Texture2D::CreateFromImage(presentTexProps, m_TestFramebuffer->GetAttachmentImage(0));
        m_PresentShader->SetTextureInput(0, m_PresentTexture, "sceneTexture");
        m_PresentShader->BakeInput();

        PipelineProperties presentPipelineProps = {};
        presentPipelineProps.shader = m_PresentShader;
        presentPipelineProps.framebuffer = m_SwapchainFramebuffer;
        presentPipelineProps.debugName = "Present Pipeline";
        presentPipelineProps.layout = { { VertexBufferDataType::Float2 }, { VertexBufferDataType::Float2 } };
        presentPipelineProps.cullMode = CullMode::None;
        m_PresentPipeline = Pipeline::Create(presentPipelineProps);

        //////////////////////////
		Shared<Shader> uvShader = Shader::Create("Quad Shader", "uvShader.vert", "uvShader.frag");

        PipelineProperties testPipelineProps = {};
        testPipelineProps.shader = uvShader;
        testPipelineProps.framebuffer = m_TestFramebuffer; // same offscreen target
        testPipelineProps.debugName = "Test Pipeline";
        testPipelineProps.layout = { { VertexBufferDataType::Float2 }, { VertexBufferDataType::Float2 } }; // matches m_QuadVB/m_QuadIB
        testPipelineProps.cullMode = CullMode::None;
        m_TestPipeline = Pipeline::Create(testPipelineProps);

        struct QuadVertex { glm::vec2 pos; glm::vec2 uv; };
        std::vector<QuadVertex> quadVerts = {
            {{-1.f, -1.f}, {0.f, 0.f}}, {{ 1.f, -1.f}, {1.f, 0.f}},
            {{ 1.f,  1.f}, {1.f, 1.f}}, {{-1.f,  1.f}, {0.f, 1.f}},
        };
        std::vector<QuadVertex> secondQuadVerts = {
            {{ 0.f, -1.f}, {0.f, 0.f}},
            {{ 0.f, -1.f}, {1.f, 0.f}},
            {{ 1.f,  1.f}, {1.f, 1.f}},
            {{-1.f,  1.f}, {0.f, 1.f}},
        };
        std::vector<uint32_t> quadIndices = { 0, 1, 2, 2, 3, 0 };
        m_QuadVB = VertexBuffer::Create(quadVerts.data(), (uint32_t)(sizeof(QuadVertex) * quadVerts.size()), VertexBufferUsage::Static);
        m_SecondQuadVB = VertexBuffer::Create(secondQuadVerts.data(), (uint32_t)(sizeof(QuadVertex) * secondQuadVerts.size()), VertexBufferUsage::Static);
        m_QuadIB = IndexBuffer::Create(quadIndices.data(), (uint32_t)quadIndices.size(), IndexBufferUsage::Static);

        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(sizeof(m_Vertices[0]) * m_Vertices.size()), VertexBufferUsage::Static);
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size(), IndexBufferUsage::Static);
        m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());
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
            m_TestFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_SwapchainFramebuffer->Resize(GetWindow().GetWidth(), GetWindow().GetHeight());
            m_Camera.SetViewportSize((float)GetWindow().GetWidth(), (float)GetWindow().GetHeight());

            TextureProperties presentTexProps = {};
            presentTexProps.debugName = "Present Texture";
            m_PresentTexture = Texture2D::CreateFromImage(presentTexProps, m_TestFramebuffer->GetAttachmentImage(0));
            m_PresentShader->SetTextureInput(0, m_PresentTexture, "sceneTexture");

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

        Renderer::BeginRenderPass(m_ScenePipeline);
        Renderer::DrawIndexed(m_VertexBuffer, m_IndexBuffer);
        Renderer::EndRenderPass();

        Renderer::BeginRenderPass(m_TestPipeline);
        Renderer::DrawIndexed(m_SecondQuadVB, m_QuadIB); // reusing the quad layout
        Renderer::EndRenderPass();

        Renderer::BeginRenderPass(m_PresentPipeline);
        Renderer::DrawIndexed(m_QuadVB, m_QuadIB);
        Renderer::EndRenderPass();

        //TODO: (in order)
        // Instanced rendering (for quads at least)
    }

#ifdef PX_INCLUDE_IMGUI
    void OnImGuiRender(float deltaTime)
    {
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Color", glm::value_ptr(m_TestColor));
        ImGui::DragFloat3("Position", glm::value_ptr(m_Camera.GetPosition()));
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
    createProps.graphicsAPI = GraphicsAPIType::Vulkan;
    return new SandboxApp(createProps, args);
}