#include <Paradox.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class SandboxApp : public Paradox::Application
{
public:
    void Run()
    {
        try 
        {
            InitWindow();
            InitVulkan();
            MainLoop();
            Cleanup();
        }
        catch (const std::exception& e)
        {
            PX_CRITICAL("An error occured: {0}", e.what());
        }
    }

private:
    GLFWwindow* m_Window = nullptr;
    VkInstance m_Instance;

private:
    void InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Paradox", nullptr, nullptr);
        PX_INFO("Created Window");
    }

    void InitVulkan()
    {
        CreateInstance();
    }

    void CreateInstance()
    {
        VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Sandbox";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Paradox";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create Vulkan Instance.");
    
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        PX_INFO("Available Vulkan Extensions:");
        for (const VkExtensionProperties& ex : extensions)
        {
            PX_INFO("\t {0} v{1}", ex.extensionName, ex.specVersion);
        }

        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            if (!std::any_of(extensions.begin(), extensions.end(), [ext = glfwExtensions[i]](const VkExtensionProperties& e) { return std::string_view(e.extensionName) == ext; }))
                PX_WARN("Extension {0} is required but not supported.", glfwExtensions[i]);
        }

    } 

    void MainLoop()
    {
        PX_INFO("Polling");
        while (!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();
		}
    }

    void Cleanup()
    {
        PX_INFO("Cleanup");
        vkDestroyInstance(m_Instance, nullptr);
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
};

Paradox::Application* Paradox::CreateApplication()
{
    return new SandboxApp();
}