#pragma once

#include "Paradox/Renderer/IndexBuffer.h"

namespace Paradox
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count, IndexBufferUsage usage);
		OpenGLIndexBuffer(const void* data, uint32_t count, IndexBufferUsage usage);
		~OpenGLIndexBuffer() override;

		void SetData(const void* data, uint32_t size) override;

		void Bind();
		void Unbind();

		uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Count;
		IndexBufferUsage m_Usage;
	};
}