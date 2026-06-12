#include "pxpch.h"
#include "VulkanBuffer.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	VulkanBuffer::VulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propFlags)
        : m_Size(size)
	{
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;  
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usageFlags;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK_RESULT(vkCreateBuffer(VulkanDevice::Get().GetDevice(), &bufferCreateInfo, nullptr, &m_Buffer));

        VkMemoryRequirements memRequirements = {};
        vkGetBufferMemoryRequirements(VulkanDevice::Get().GetDevice(), m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(memRequirements.memoryTypeBits, propFlags);

        VK_CHECK_RESULT(vkAllocateMemory(VulkanDevice::Get().GetDevice(), &allocInfo, nullptr, &m_BufferMemory));
        VK_CHECK_RESULT(vkBindBufferMemory(VulkanDevice::Get().GetDevice(), m_Buffer, m_BufferMemory, 0));
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

        VK_CHECK_RESULT(vkMapMemory(device, m_BufferMemory, 0, size, 0, &mappedData));
        memcpy(mappedData, data, (size_t)size);
        vkUnmapMemory(device, m_BufferMemory);

        m_Size = size;
    }
}