#include "pxpch.h"
#include "VitaWindow.h"

#include <vitaGL.h>

namespace Paradox {

    VitaWindow::VitaWindow(const WindowCreateProperties &props)
    {
        m_WindowData.title = props.title;
        m_WindowData.width = 960;
        m_WindowData.height = 544;
        m_WindowData.fullscreen = props.fullscreen;
    }

    VitaWindow::~VitaWindow()
    {
        PX_CORE_INFO("Destroyed Window {0}", m_WindowData.title);
    }

    void VitaWindow::Init()
    {
        m_GraphicsContext = GraphicsContext::Create();
        m_GraphicsContext->Init();

        bool vsyncEnabled = false;
        m_SwapChain = SwapChain::Create();
        m_SwapChain->Init(this);
        m_SwapChain->Create(m_WindowData.width, m_WindowData.height, vsyncEnabled);

        PX_CORE_INFO("Created Window {0} {1}x{2}", m_WindowData.title, m_WindowData.width, m_WindowData.height);
    }

    void VitaWindow::OnUpdate()
    {
        vglSwapBuffers(false);
    }
}
