#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>

namespace Paradox
{
	// PARADOX_API is Temporary
	class PARADOX_API VulkanContext : public GraphicsContext
	{
	public:
		~VulkanContext();

		void Init() override;
		void WaitIdle() override;

		// Temporary
		static VkInstance GetVkInstance() { return s_Instance; }

	private:
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		void SetupDebugMessenger();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
			VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* userData);

	private:
		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		static VkInstance s_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		std::vector<VkExtensionProperties> m_InstanceExtensions;

#ifdef PX_DEBUG
		const bool m_ValidationLayersEnabled = true;
#else
		const bool m_ValidationLayersEnabled = false;
#endif
	};
}