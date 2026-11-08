#include "RuntimeApp.h"

#include <Paradox/Core/EntryPoint.h>

using namespace Paradox;

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    RuntimeConfig runtimeConfig;
    bool success = runtimeConfig.Deserialize(std::filesystem::current_path() / "Assets" / "App.bin");
    PX_ASSERT(success, "Failed to read RuntimeConfig.");

    RuntimeConfigProperties& runtimeProps = runtimeConfig.GetProperties();
#ifndef PX_DIST
    std::stringstream ss;
    ss << runtimeProps.windowProperties.title << " (" << PX_BUILD_CONFIG_NAME << ")";
    runtimeProps.windowProperties.title = ss.str();
#endif
    /*WindowCreateProperties createProps;
    createProps.title = "Runtime";
    createProps.width = 1280;
    createProps.height = 720;
    createProps.graphicsAPI = GraphicsAPIType::Vulkan;*/
    return new RuntimeApp(runtimeProps, args);
}