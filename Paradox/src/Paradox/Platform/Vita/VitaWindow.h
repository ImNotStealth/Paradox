#pragma once

#include "Paradox/Core/Window.h"

namespace Paradox
{
    class VitaWindow : public Window
    {
    public:
        VitaWindow(const WindowCreateProperties& props);
        ~VitaWindow() override;

        void Init() override;
        void OnUpdate() override;

        const std::string& GetWindowTitle() const override { return m_WindowData.title; }
        uint32_t GetWidth() const override { return m_WindowData.width; }
        uint32_t GetHeight() const override { return m_WindowData.height; }
		bool IsVSync() const override { return true; }
        void* GetHandle() const override { return nullptr; }
        Shared<GraphicsContext> GetGraphicsContext() override { return m_GraphicsContext; }
        Shared<SwapChain> GetSwapChain() override { return m_SwapChain; }

        //TODO: Temporary
        void WaitEvents() override {}

        void SetEventCallback(const EventCallbackFn& callback) override { m_WindowData.eventCallback = callback; }
        void SetVSync(bool enabled) override {}

    private:
        struct VitaWindowData
        {
            std::string title;
            uint32_t width, height;
            bool fullscreen;
            EventCallbackFn eventCallback;
        };

        VitaWindowData m_WindowData;

        Shared<GraphicsContext> m_GraphicsContext;
        Shared<SwapChain> m_SwapChain;
    };
}