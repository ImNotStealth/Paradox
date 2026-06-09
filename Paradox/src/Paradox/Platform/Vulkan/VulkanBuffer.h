#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Platform/Vulkan/Vulkan.h"

namespace Paradox
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propFlags);
		~VulkanBuffer();

		void SetData(const void* data, VkDeviceSize size);

		VkBuffer& GetBuffer() { return m_Buffer; }
		VkDeviceSize GetSize() { return m_Size; }

	private:
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
		VkDeviceSize m_Size;
	};
}