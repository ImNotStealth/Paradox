#include <Paradox.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

class SandboxApp : public Paradox::Application
{
public:
    SandboxApp()
    {

    }

    ~SandboxApp()
    {

    }

    void Run()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(800, 600, "Paradox", nullptr, nullptr);

        uint32_t extensions = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensions, nullptr);

        PX_INFO("Got {0} extensions", extensions);

        glm::mat4 matrix;
        glm::vec4 vec;
        auto test = matrix * vec;

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

Paradox::Application* Paradox::CreateApplication()
{
    return new SandboxApp();
}