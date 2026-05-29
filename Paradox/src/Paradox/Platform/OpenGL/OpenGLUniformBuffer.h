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

		template<typename T>
		T* GetData() { return reinterpret_cast<T*>(m_CPUBuffer.data()); }

		uint32_t GetBufferID() { return m_BufferID; }
	
	private:
		uint32_t m_BufferID;
		std::vector<uint8_t> m_CPUBuffer;
	};
}