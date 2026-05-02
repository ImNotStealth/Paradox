#include "pxpch.h"
#include "VulkanDevice.h"

#include "Paradox/Platform/Vulkan/VulkanContext.h"
#include "Paradox/Core/Application.h"

namespace Paradox
{
	VulkanDevice* VulkanDevice::s_Instance;

	VulkanDevice::~VulkanDevice()
	{
		PX_CORE_TRACE("Destroying Vulkan Device");
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		vkDestroyDevice(m_Device, nullptr);
	}

	void VulkanDevice::Init()
	{
		FindPhysicalDevice();
		CreateLogicalDevice();

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = m_FamilyIndices.graphicsFamily;

		VkResult result = vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPool);
		PX_ASSERT(result == VK_SUCCESS, "Failed to create Command Pool.");
	}

	VkCommandBuffer VulkanDevice::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer cmdBuffer = {};
		vkAllocateCommandBuffers(m_Device, &allocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);
		return cmdBuffer;
	}

	void VulkanDevice::EndSingleTimeCommands(VkCommandBuffer cmdBuffer)
	{
		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);
		vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &cmdBuffer);
	}
	
	void VulkanDevice::FindPhysicalDevice()
	{
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(VulkanContext::GetVkInstance(), &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			PX_CORE_CRITICAL("No Physical Devices found (or they don't support Vulkan).");
			return;
		}

		std::vector<VkPhysicalDevice> physicalDevices;
		physicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(VulkanContext::GetVkInstance(), &deviceCount, physicalDevices.data());

		std::vector<PhysicalDeviceInfo> deviceInfos;
		for (const VkPhysicalDevice& device : physicalDevices)
			deviceInfos.push_back(GetPhysicalDeviceInfo(device));

		// Sort by type, then by most VRAM
		std::sort(deviceInfos.begin(), deviceInfos.end(), [](PhysicalDeviceInfo& a, PhysicalDeviceInfo& b)
			{
				if (a.deviceType == b.deviceType)
					return a.memory > b.memory;
				
				return a.deviceType < b.deviceType;
			});

		PX_CORE_TRACE("Found Physical Devices:");
		for (const PhysicalDeviceInfo& info : deviceInfos)
			PX_CORE_TRACE("\t{0}: {1}, {2} MB", info.name, PhysicalDeviceTypeToString(info.deviceType), info.memory);

		m_PhysicalDeviceInfo = deviceInfos[0];
		m_PhysicalDevice = m_PhysicalDeviceInfo.deviceHandle;

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			PX_CORE_CRITICAL("Failed to find suitable GPU.");
			return;
		}

		PX_CORE_INFO("Selected Physical Device: {0}: {1}, {2} MB", m_PhysicalDeviceInfo.name, PhysicalDeviceTypeToString(m_PhysicalDeviceInfo.deviceType), m_PhysicalDeviceInfo.memory);
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		//PX_CORE_TRACE("Available Device Extensions:");
		for (const VkExtensionProperties& ext : availableExtensions)
		{
			//PX_CORE_TRACE("\t{0} v{1}", ext.extensionName, ext.specVersion);
			requiredExtensions.erase(ext.extensionName);
		}

		for (const std::string& s : requiredExtensions)
			PX_CORE_WARN("Extension not supported: {0}", s);

		m_FamilyIndices = FindQueueFamilies();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<int32_t> uniqueQueueFamilies = { m_FamilyIndices.graphicsFamily, m_FamilyIndices.transferFamily };

		float queuePriority = 1.f;
		for (int32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.fillModeNonSolid = true;
		deviceFeatures.wideLines = true;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		Shared<VulkanContext> context = std::static_pointer_cast<VulkanContext>(Application::Get().GetWindow().GetGraphicsContext());
		if (context->ValidationLayersEnabled)
		{
			createInfo.enabledLayerCount = (uint32_t)context->ValidationLayers.size();
			createInfo.ppEnabledLayerNames = context->ValidationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
		PX_ASSERT(result == VK_SUCCESS, "Failed to create Logical Device.");
		PX_CORE_TRACE("Created Vulkan Device");

		vkGetDeviceQueue(m_Device, m_FamilyIndices.graphicsFamily, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_FamilyIndices.graphicsFamily, 0, &m_PresentQueue);
		vkGetDeviceQueue(m_Device, m_FamilyIndices.transferFamily, 0, &m_TransferQueue);
	}

	VulkanDevice::PhysicalDeviceInfo VulkanDevice::GetPhysicalDeviceInfo(const VkPhysicalDevice& device)
	{
		PhysicalDeviceInfo info = {};
		
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

		uint64_t memoryBytes = (uint64_t)memoryProperties.memoryHeaps[0].size;
		uint32_t memoryMb = memoryBytes / 1024 / 1024;

		PhysicalDeviceType type = PhysicalDeviceType::Unknown;
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			type = PhysicalDeviceType::Discrete;
		else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			type = PhysicalDeviceType::Integrated;
		else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
			type = PhysicalDeviceType::Virtual;
		else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
			type = PhysicalDeviceType::CPU;

		info.name = std::string(properties.deviceName);
		info.memory = memoryMb;
		info.deviceType = type;
		info.deviceHandle = device;

		return info;
	}

	std::string VulkanDevice::PhysicalDeviceTypeToString(PhysicalDeviceType type)
	{
		switch (type)
		{
			case PhysicalDeviceType::Discrete: return "Discrete";
			case PhysicalDeviceType::Integrated: return "Integrated";
			case PhysicalDeviceType::Virtual: return "Virtual";
			case PhysicalDeviceType::CPU: return "CPU";
			default: return "Unknown";
		}
	}

	VulkanDevice::QueueFamilyIndices VulkanDevice::FindQueueFamilies()
	{
		QueueFamilyIndices indices = {};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

		// First look for a dedicated transfer queue
		// Then fallback to any queue that supports transfer if no dedicated one has been found
		// For graphics any one is valid

		int i = 0;
		for (const VkQueueFamilyProperties& prop : queueFamilies)
		{
			if (prop.queueFlags & VK_QUEUE_TRANSFER_BIT && (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
			{
				indices.transferFamily = i;
				break;
			}
			i++;
		}

		i = 0;
		for (const VkQueueFamilyProperties& prop : queueFamilies)
		{
			if (prop.queueFlags & VK_QUEUE_TRANSFER_BIT && indices.transferFamily == -1)
				indices.transferFamily = i;

			if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			if (indices.IsComplete())
				break;
		}

		return indices;
	}
}