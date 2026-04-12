#pragma once

#include "Paradox/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Paradox
{
	class PARADOX_API VulkanDevice
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t GraphicsFamily = -1;
			int32_t TransferFamily = -1;

			bool IsComplete()
			{
				return GraphicsFamily != -1 && TransferFamily != -1;
			}
		};

		~VulkanDevice();

		void Init();

		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkDevice GetDevice() { return m_Device; }
		QueueFamilyIndices GetQueueFamilyIndices() { return m_FamilyIndices; }

		//Temporary
		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() { return m_PresentQueue; }

		static VulkanDevice& Get()
		{
			if (!s_Instance)
				s_Instance = new VulkanDevice();

			return *s_Instance;
		}

		static void Shutdown()
		{
			if (s_Instance)
			{
				delete s_Instance;
				s_Instance = nullptr;
			}
		}

	private:
		enum PhysicalDeviceType
		{
			Discrete = 0,
			Integrated,
			Virtual,
			CPU,
			Unknown
		};
		struct PhysicalDeviceInfo
		{
			uint32_t m_Memory = 0;
			std::string m_Name;
			PhysicalDeviceType m_Type = PhysicalDeviceType::Unknown;
			VkPhysicalDevice m_Handle = VK_NULL_HANDLE;
		};

		void FindPhysicalDevice();
		void CreateLogicalDevice();

		PhysicalDeviceInfo GetPhysicalDeviceInfo(const VkPhysicalDevice& device);
		std::string PhysicalDeviceTypeToString(PhysicalDeviceType type);
		QueueFamilyIndices FindQueueFamilies();

	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		PhysicalDeviceInfo m_PhysicalDeviceInfo;
		VkDevice m_Device = VK_NULL_HANDLE;
		QueueFamilyIndices m_FamilyIndices;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkQueue m_TransferQueue;

		static VulkanDevice* s_Instance;
	};
}