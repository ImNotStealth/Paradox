#pragma once

#include "Paradox/Renderer/VertexBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"

namespace Paradox
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t size, VertexBufferUsage usage);
		VulkanVertexBuffer(const void* data, uint32_t size, VertexBufferUsage usage);
	
		void SetData(const void* data, uint32_t size) override;

		//TEMP
		VkBuffer& GetBuffer() { return m_DeviceBuffer->GetBuffer(); }

	private:
		void CopyToDevice(const void* data, uint32_t size);

	private:
		Shared<VulkanBuffer> m_DeviceBuffer;
		VertexBufferUsage m_Usage;
	};
}