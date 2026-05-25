#include "pxpch.h"
#include "VulkanUniformBuffer.h"

namespace Paradox
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
	{
		m_Buffer = CreateShared<VulkanBuffer>(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size)
	{
		m_Buffer->SetData(data, size);
	}
}