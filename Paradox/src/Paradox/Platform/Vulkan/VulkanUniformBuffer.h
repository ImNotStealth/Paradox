#pragma once

#include "Paradox/Renderer/UniformBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"

namespace Paradox
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size);

		void SetData(const void* data, uint32_t size) override;

		Shared<VulkanBuffer> GetBuffer() { return m_Buffer; }

	private:
		Shared<VulkanBuffer> m_Buffer;
	};
}