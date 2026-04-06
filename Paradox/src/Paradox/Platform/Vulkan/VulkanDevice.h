#pragma once

#include "Paradox/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Paradox
{
	class PARADOX_API VulkanDevice
	{
	public:
		void Init();

		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkDevice GetDevice() { return m_Device; }

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
		struct QueueFamilyIndices
		{
			int32_t m_GraphicsFamily = -1;
			int32_t m_TransferFamily = -1;

			bool IsComplete()
			{
				return m_GraphicsFamily != -1 && m_TransferFamily != -1;
			}
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
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkQueue m_TransferQueue;

		static VulkanDevice* s_Instance;
	};
}