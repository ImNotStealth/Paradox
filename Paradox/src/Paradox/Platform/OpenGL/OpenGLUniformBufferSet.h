#pragma once

#include "Paradox/Renderer/UniformBufferSet.h"

namespace Paradox
{
	// OpenGL doesn't have the concept of multiple frames in flight
	// so this just wraps a regular UniformBuffer
	class OpenGLUniformBufferSet : public UniformBufferSet
	{
	public:
		OpenGLUniformBufferSet(uint32_t size);

		Shared<UniformBuffer> GetCurrent() override { return m_UniformBuffer; }
		Shared<UniformBuffer> Get(uint32_t frameIndex) override { return m_UniformBuffer; }

	private:
		Shared<UniformBuffer> m_UniformBuffer;
	};
}