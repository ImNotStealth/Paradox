#pragma once

#include "Paradox/Renderer/UniformBufferSet.h"

namespace Paradox
{
	class VulkanUniformBufferSet : public UniformBufferSet
	{
	public:
		VulkanUniformBufferSet(uint32_t size);

		Shared<UniformBuffer> GetCurrent() override;
		Shared<UniformBuffer> Get(uint32_t frameIndex) { return m_UniformBuffers[frameIndex]; }

	private:
		std::vector<Shared<UniformBuffer>> m_UniformBuffers;
	};
}