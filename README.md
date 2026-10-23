<div align="center">
    <img src="/WorkingDirectories/Paradox-Editor/Assets/Textures/Paradox_Full_256x.png" alt="Paradox"/>
</div>

A 2D Game Engine with support for Windows, Linux and PlayStation Vita using Vulkan and OpenGL.

# Getting Started
Most libraries that the engine uses can be toggled in the `CMakeLists.txt` file. Options provided by the engine will have a `PX_` prefix.

Here are some things that you might need before compiling:
- [Git](https://git-scm.com/)
- [CMake](https://cmake.org), any version should work but version `4.5.0-rc1` is known to be working.
- [VulkanSDK](https://vulkan.lunarg.com/sdk/home), The engine only demands API version 1.0 but it has been tested with version `1.4.341.1`.
  - Since the engine also supports OpenGL, you can skip installing the VulkanSDK by disabling the `PX_INCLUDE_VULKAN` option.
  - Also make sure to run the `CompileShaders.sh` in the `WorkingDirectories/PROJECT/Assets/Shaders/Desktop` folder to generate the SPIR-V files.
- [VitaSDK](https://vitasdk.org/), if you're planning on compiling for the PS Vita (Linux only as of now).
 
Once you have everything set up, start by cloning this repository:
```sh
git clone https://github.com/ImNotStealth/Paradox.git
```

### Compiling for Windows
Run the `scripts/GenProjects.sh` file to generate a solution for Visual Studio (I haven't tried other IDEs yet).

### Compiling for Linux
I only used CLion so I just opened the folder as a project and it detected the CMake project directly.

### Compiling for PS Vita (Linux only)
If you want to build for PS Vita, you just need to run the `scripts/CompileVita.sh` file and it'll build the Runtime-Vita project into a `build-vita` folder with your .vpk inside.

Below are all the build options for the engine:
| Option | Description |
|--------|-------------|
| PX_BUILD_DLL | Should the engine be built into a separate .dll file or included in the application's .exe |
| PX_INCLUDE_SPDLOG | Includes spdlog or uses a fallback logger instead (still keeps the same format as spdlog) |
| PX_INCLUDE_GLFW | Includes GLFW, required when compiling for Desktop |
| PX_INCLUDE_VULKAN | Includes Vulkan, VulkanSDK is required, this option may be disabled but you'll need to have PX_INCLUDE_OPENGL enabled as you need at least one renderer |
| PX_INCLUDE_OPENGL | Includes OpenGL, may be disabled but you'll need to have PX_INCLUDE_VULKAN enabled as you need at least one renderer |
| PX_INCLUDE_GLAD | Includes GLAD, required if you're using OpenGL and compiling for Desktop |
| PX_INCLUDE_IMGUI | Includes ImGui, required when building the Editor, but optional when you're building the Runtime |
| PX_INCLUDE_VITASDK | Includes the VitaSDK, this option is only enabled by the `CompileVita.sh` script so you'll never need to enable this manually |
| PX_INCLUDE_TRACY | Includes Tracy, can be safely disabled if you don't need profiling |

PS: On Windows, if you changed an option after generating the solution, you'll need to run the `scripts/GenProjects.sh` file again to regenerate the solution and apply your options.
