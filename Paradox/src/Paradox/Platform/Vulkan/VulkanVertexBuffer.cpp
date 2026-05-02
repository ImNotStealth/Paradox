#include "pxpch.h"
#include "VulkanVertexBuffer.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"

namespace Paradox
{
	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		VkMemoryPropertyFlags memFlags = usage == VertexBufferUsage::Dynamic ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		m_DeviceBuffer = CreateShared<VulkanBuffer>(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memFlags);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32_t size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		VkMemoryPropertyFlags memFlags = usage == VertexBufferUsage::Dynamic ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		m_DeviceBuffer = CreateShared<VulkanBuffer>(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memFlags);
		
		if (usage == VertexBufferUsage::Dynamic)
			m_DeviceBuffer->SetData(data, size);
		else
			CopyToDevice(data, size);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		PX_CORE_ASSERT(m_Usage == VertexBufferUsage::Dynamic, "Only Dynamic Vertex Buffers can be updated.");
		m_DeviceBuffer->SetData(data, size);
	}

	void VulkanVertexBuffer::CopyToDevice(const void* data, uint32_t size)
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