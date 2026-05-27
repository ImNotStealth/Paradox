#pragma once

#include "Paradox/Renderer/UniformBuffer.h"

namespace Paradox
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size);
		~OpenGLUniformBuffer();

		void SetData(const void* data, uint32_t size) override;

		uint32_t GetBufferID() { return m_BufferID; }
	
	private:
		uint32_t m_BufferID;
	};
}