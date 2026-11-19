#include "RuntimeVitaApp.h"

#include <Paradox/Core/EntryPoint.h>
#include <Paradox/Renderer/GraphicsContext.h>
#include <Paradox/Core/RuntimeConfig.h>

using namespace Paradox;

Application* Paradox::CreateApplication(const CommandLineParser& args)
{
    RuntimeConfig runtimeConfig;
    bool success = runtimeConfig.Deserialize(std::filesystem::current_path() / "Assets" / "App.bin");
    PX_ASSERT(success, "Failed to read RuntimeConfig.");

    RuntimeConfigProperties& runtimeProps = runtimeConfig.GetProperties();
    return new RuntimeVitaApp(runtimeProps, args);
}