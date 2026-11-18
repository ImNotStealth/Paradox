#include "RuntimeVitaApp.h"

#include <Paradox/Core/EntryPoint.h>
#include <Paradox/Renderer/GraphicsContext.h>

using namespace Paradox;

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    WindowCreateProperties createProps;
    createProps.title = "Runtime-Vita";
    createProps.graphicsAPI = GraphicsAPIType::OpenGL;
    createProps.width = 960;
    createProps.height = 544;
    return new RuntimeVitaApp(createProps, args);
}