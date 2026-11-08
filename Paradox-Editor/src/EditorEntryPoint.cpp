#include "EditorApp.h"

#include "Utils/EditorConfig.h"

#include <Paradox/Core/EntryPoint.h>

using namespace Paradox;

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    EditorConfig::Deserialize();
    WindowCreateProperties createProps;
    createProps.title = "Paradox Editor";
    createProps.width = 1280;
    createProps.height = 720;
	createProps.iconPath = "Assets/Paradox.png";
	createProps.graphicsAPI = EditorConfig::GetProperties().graphicsApi;
    createProps.vsync = EditorConfig::GetProperties().vsync;
    return new EditorApp(createProps, args);
}