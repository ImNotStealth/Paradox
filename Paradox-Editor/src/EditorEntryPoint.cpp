#include "EditorApp.h"

#include <Paradox/Core/EntryPoint.h>

using namespace Paradox;

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    WindowCreateProperties createProps;
    createProps.title = "Paradox Editor";
    createProps.width = 1280;
    createProps.height = 720;
	createProps.graphicsAPI = GraphicsAPIType::OpenGL;
    return new EditorApp(createProps, args);
}