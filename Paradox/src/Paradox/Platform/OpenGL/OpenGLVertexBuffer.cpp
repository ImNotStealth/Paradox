#include "pxpch.h"
#include "OpenGLVertexBuffer.h"

#include "Paradox/Platform/OpenGL/OpenGL.h"

namespace Paradox
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		PX_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, usage == VertexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		PX_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage == VertexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		PX_PROFILE_FUNCTION();
		PX_CORE_ASSERT(m_Usage == VertexBufferUsage::Dynamic, "Only Dynamic Vertex Buffers can be updated.");
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}