#include "pxpch.h"
#include "VulkanIndexBuffer.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count, IndexBufferUsage usage)
		: m_Count(count), m_Usage(usage)
	{
		VkMemoryPropertyFlags memFlags = usage == IndexBufferUsage::Dynamic ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		m_DeviceBuffer = CreateShared<VulkanBuffer>(count * sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memFlags);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(const void* data, uint32_t count, IndexBufferUsage usage)
		: m_Count(count), m_Usage(usage)
	{
		VkMemoryPropertyFlags memFlags = usage == IndexBufferUsage::Dynamic ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VkDeviceSize size = count * sizeof(uint32_t);
		m_DeviceBuffer = CreateShared<VulkanBuffer>(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memFlags);
	
		if (usage == IndexBufferUsage::Dynamic)
			m_DeviceBuffer->SetData(data, size);
		else
			CopyToDevice(data, size);
	}

	void VulkanIndexBuffer::SetData(const void* data, uint32_t count)
	{
		PX_CORE_ASSERT(m_Usage == IndexBufferUsage::Dynamic, "Only Dynamic Index Buffers can be updated.");
		m_DeviceBuffer->SetData(data, count * sizeof(uint32_t));
	}

	void VulkanIndexBuffer::CopyToDevice(const void* data, uint32_t size)
	{
		VulkanBuffer stagingBuffer = VulkanBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.SetData(data, size);

		VkCommandBuffer cmdBuffer = VulkanDevice::Get().BeginSingleTimeCommands();
		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(cmdBuffer, stagingBuffer.GetBuffer(), m_DeviceBuffer->GetBuffer(), 1, &copyRegion);

		VulkanDevice::Get().EndSingleTimeCommands(cmdBuffer);
	}
}