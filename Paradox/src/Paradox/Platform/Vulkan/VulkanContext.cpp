#include "pxpch.h"
#include "VulkanContext.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"

// Since we're running Vulkan, we can assume we're on PC and are using GLFW (for now).
#include <GLFW/glfw3.h>

namespace Paradox
{
    VkInstance VulkanContext::s_Instance;

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (!func)
            return VK_ERROR_EXTENSION_NOT_PRESENT;

        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(instance, debugMessenger, pAllocator);
    }

    VulkanContext::~VulkanContext()
    {
        if (ValidationLayersEnabled)
            DestroyDebugUtilsMessengerEXT(s_Instance, m_DebugMessenger, nullptr);

        VulkanDevice::Shutdown();
        vkDestroyInstance(s_Instance, nullptr);
    }

    void VulkanContext::Init()
	{
        PX_CORE_ASSERT(!ValidationLayersEnabled || CheckValidationLayerSupport(), "Validation layers requested but not available.");

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        //TODO: Get name from client application
        appInfo.pApplicationName = Application::Get().GetWindow().GetWindowTitle().c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Paradox";

        std::string engineVersion = PX_VERSION;
        std::replace(engineVersion.begin(), engineVersion.end(), '.', ' ');
        std::istringstream ss(engineVersion);
        auto versionSplit = std::vector<std::string>(std::istream_iterator<std::string>(ss), {});
        appInfo.engineVersion = VK_MAKE_VERSION(std::stoi(versionSplit[0]), std::stoi(versionSplit[1]), std::stoi(versionSplit[2]));
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        if (ValidationLayersEnabled)
        {
            createInfo.enabledLayerCount = (uint32_t)ValidationLayers.size();
            createInfo.ppEnabledLayerNames = ValidationLayers.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
            createInfo.enabledLayerCount = 0;

        std::vector<const char*> requiredExtensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &s_Instance);
        PX_CORE_ASSERT(instanceResult == VK_SUCCESS, "Failed to create Vulkan Instance.");

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        //PX_CORE_TRACE("Available Instance Extensions:");
        //for (const VkExtensionProperties& ex : extensions)
        //    PX_CORE_TRACE("\t {0} v{1}", ex.extensionName, ex.specVersion);

        //PX_CORE_TRACE("Required Extensions:");
        for (const char* extension : requiredExtensions)
        {
            //PX_CORE_TRACE("\t {0}", extension);
            if (!std::any_of(extensions.begin(), extensions.end(), [extension](const VkExtensionProperties& e) { return std::strcmp(e.extensionName, extension) == 0; }))
                PX_CORE_WARN("Extension {0} is required but not supported.", extension);
        }

        VulkanDevice::Get().Init();
	}

    void VulkanContext::WaitIdle()
    {
        vkDeviceWaitIdle(VulkanDevice::Get().GetDevice());
    }

    std::vector<const char*> VulkanContext::GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> requiredExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (ValidationLayersEnabled)
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return requiredExtensions;
    }

    bool VulkanContext::CheckValidationLayerSupport()
    {
        uint32_t validationLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(validationLayerCount);
        vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

        for (const char* layerName : ValidationLayers)
        {
            if (!std::any_of(availableLayers.begin(), availableLayers.end(), [layerName](const VkLayerProperties& e) { return std::strcmp(e.layerName, layerName) == 0; }))
            {
                PX_CORE_WARN("Validation layer {0} is requested but not supported.", layerName);
                return false;
            }
        }

        return true;
    }

    void VulkanContext::SetupDebugMessenger()
    {
        if (!ValidationLayersEnabled)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        VkResult result = CreateDebugUtilsMessengerEXT(s_Instance, &createInfo, nullptr, &m_DebugMessenger);
        PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to setup Debug Messenger.");
    }

    void VulkanContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
        VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* userData)
    {
        switch (msgSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            PX_CORE_TRACE("Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            PX_CORE_INFO("Validation Layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            PX_CORE_WARN("Validation Layer: {0}", pCallbackData->pMessage);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        default:
            PX_CORE_ERROR("Validation Layer: {0}", pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }
}