#include "pxpch.h"
#include "VulkanBuffer.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	VulkanBuffer::VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propFlags)
	{
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult bufferResult = vkCreateBuffer(VulkanDevice::Get().GetDevice(), &bufferCreateInfo, nullptr, &m_Buffer);
        PX_ASSERT(bufferResult == VK_SUCCESS, "Failed to create Buffer.");

        VkMemoryRequirements memRequirements = {};
        vkGetBufferMemoryRequirements(VulkanDevice::Get().GetDevice(), m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, propFlags);

        VkResult allocateResult = vkAllocateMemory(VulkanDevice::Get().GetDevice(), &allocInfo, nullptr, &m_BufferMemory);
        PX_ASSERT(allocateResult == VK_SUCCESS, "Failed to allocate Buffer memory.");

        vkBindBufferMemory(VulkanDevice::Get().GetDevice(), m_Buffer, m_BufferMemory, 0);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
        vkDestroyBuffer(device, m_Buffer, nullptr);
        vkFreeMemory(device, m_BufferMemory, nullptr);
	}

    void VulkanBuffer::SetData(const void* data, VkDeviceSize size)
    {
        VkDevice device = VulkanDevice::Get().GetDevice();
        void* mappedData;

        vkMapMemory(device, m_BufferMemory, 0, size, 0, &mappedData);
        memcpy(mappedData, data, (size_t)size);
        vkUnmapMemory(device, m_BufferMemory);
    }

    uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties = {};
        vkGetPhysicalDeviceMemoryProperties(VulkanDevice::Get().GetPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        PX_ASSERT(false, "Failed to find suitable memory type.");
        return 0;
    }
}