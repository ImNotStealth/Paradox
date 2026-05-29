#include "pxpch.h"
#include "OpenGLUniformBuffer.h"

#include <cstring>

#include "Paradox/Platform/OpenGL/OpenGL.h"

namespace Paradox
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size)
	{
		m_CPUBuffer.resize(size);

		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size)
	{
		memcpy(m_CPUBuffer.data(), data, size);
		glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}
}