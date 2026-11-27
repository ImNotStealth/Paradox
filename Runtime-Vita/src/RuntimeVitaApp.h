#pragma once

#include <Paradox.h>
#include <Paradox/Core/RuntimeConfig.h>

using namespace Paradox;

class RuntimeVitaApp : public Application
{
public:
    RuntimeVitaApp(const RuntimeConfigProperties& runtimeProps, const CommandLineParser& args);

private:
    void OnUpdate(float deltaTime) override;

private:
    Shared<Pipeline> m_PresentPipeline = nullptr;
    Shared<Framebuffer> m_SceneFramebuffer = nullptr;
    Shared<Framebuffer> m_SwapchainFramebuffer = nullptr;
    Reference<Texture2D> m_PresentTexture = nullptr;

    Shared<VertexBuffer> m_PresentVB = nullptr;
    Shared<IndexBuffer> m_PresentIB = nullptr;

    Reference<Scene> m_Scene;
    Shared<AssetManager> m_AssetManager;
};