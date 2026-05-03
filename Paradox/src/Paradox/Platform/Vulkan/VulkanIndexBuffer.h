#pragma once

#include "Paradox/Renderer/IndexBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"

namespace Paradox
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t count, IndexBufferUsage usage);
		VulkanIndexBuffer(const void* data, uint32_t count, IndexBufferUsage usage);

		void SetData(const void* data, uint32_t count) override;
		uint32_t GetCount() const override { return m_Count; }

		//TEMP
		VkBuffer& GetBuffer() { return m_DeviceBuffer->GetBuffer(); }

	private:
		void CopyToDevice(const void* data, uint32_t size);

	private:
		uint32_t m_Count;
		IndexBufferUsage m_Usage;
		Shared<VulkanBuffer> m_DeviceBuffer;
	};
}