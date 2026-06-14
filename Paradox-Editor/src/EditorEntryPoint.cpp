#include "EditorApp.h"

#include <Paradox/Core/EntryPoint.h>

Paradox::Application* Paradox::CreateApplication()
{
    WindowCreateProperties createProps;
    createProps.title = "Paradox Editor";
    createProps.width = 1280;
    createProps.height = 720;
    return new EditorApp(createProps);
}