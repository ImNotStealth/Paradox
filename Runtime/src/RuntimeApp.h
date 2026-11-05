#pragma once

#include <Paradox.h>
#include <Paradox/Core/RuntimeConfig.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

using namespace Paradox;

class RuntimeApp : public Application
{
public:
    RuntimeApp(const RuntimeConfigProperties& runtimeProps, const CommandLineParser& args);

private:
    void OnEvent(Event& event) override;
    void OnUpdate(float deltaTime) override;

#ifdef PX_INCLUDE_IMGUI
    void OnImGuiRender(float deltaTime) override;
#endif

private:
    Camera m_Camera;

    Shared<Pipeline> m_PresentPipeline = nullptr;
    Shared<Framebuffer> m_SceneFramebuffer = nullptr;
    Shared<Framebuffer> m_SwapchainFramebuffer = nullptr;
    Reference<Texture2D> m_PresentTexture = nullptr;

    Shared<VertexBuffer> m_PresentVB = nullptr;
    Shared<IndexBuffer> m_PresentIB = nullptr;
    bool m_NeedResize = true;

    Reference<Scene> m_Scene;
    Shared<AssetManager> m_AssetManager;

    glm::vec3 m_QuadPosition = { 10.f, -10.0f, 0.f };
};