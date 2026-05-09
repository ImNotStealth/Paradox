#include "pxpch.h"
#include "OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace Paradox
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, IndexBufferUsage usage)
		: m_Count(count), m_Usage(usage)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, uint32_t count, IndexBufferUsage usage)
		: m_Count(count), m_Usage(usage)
	{
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t size)
	{
		PX_CORE_ASSERT(m_Usage == IndexBufferUsage::Dynamic, "Only Dynamic Index Buffers can be updated.");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size * sizeof(uint32_t), data);
	}

	void OpenGLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}