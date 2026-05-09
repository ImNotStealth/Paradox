#pragma once

#include "Paradox/Renderer/VertexBuffer.h"

namespace Paradox
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage);
		OpenGLVertexBuffer(const void* data, uint32_t size, VertexBufferUsage usage);
		~OpenGLVertexBuffer() override;

		void SetData(const void* data, uint32_t size) override;

		void Bind();
		void Unbind();

	private:
		uint32_t m_BufferID;
		VertexBufferUsage m_Usage;
	};
}