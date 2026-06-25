#pragma once

#include "Paradox/Renderer/IndexBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"

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
		GLenum GetIndexType() const { return m_IndexType; }

	private:
		uint32_t m_BufferID;
		uint32_t m_Count;
		GLenum m_IndexType = GL_UNSIGNED_INT;
		IndexBufferUsage m_Usage;
	};
}