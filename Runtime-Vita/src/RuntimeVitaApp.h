#pragma once

#include <Paradox.h>

using namespace Paradox;

class RuntimeVitaApp : public Application
{
public:
    RuntimeVitaApp(const WindowCreateProperties& windowProps, const CommandLineParser& args);

private:
    void OnUpdate(float deltaTime) override;

private:
    Camera m_Camera;

    Shared<Pipeline> m_PresentPipeline = nullptr;
    Shared<Framebuffer> m_SceneFramebuffer = nullptr;
    Shared<Framebuffer> m_SwapchainFramebuffer = nullptr;
    Reference<Texture2D> m_PresentTexture = nullptr;

    Shared<VertexBuffer> m_PresentVB = nullptr;
    Shared<IndexBuffer> m_PresentIB = nullptr;
    int m_SpiralCount = 5;

    Reference<Scene> m_Scene;
    Shared<AssetManager> m_AssetManager;
};