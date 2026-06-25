#include "pxpch.h"
#include "OpenGLIndexBuffer.h"

namespace Paradox
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, IndexBufferUsage usage)
		: m_Count(count), m_Usage(usage)
	{
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);

#ifdef PX_PLATFORM_PSVITA
		m_IndexType = GL_UNSIGNED_SHORT;
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint16_t), nullptr, usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
#else
		m_IndexType = GL_UNSIGNED_INT;
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
#endif

		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, uint32_t count, IndexBufferUsage usage)
		: m_Count(count), m_Usage(usage)
	{
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
		glCreateBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);

#ifdef PX_PLATFORM_PSVITA
		// VitaGL only seems to support uint16_t :/
		// So to prevent handicapping the entire engine to use uint16_t, we'll try to convert the buffer to it if we're on PS Vita.
		m_IndexType = GL_UNSIGNED_SHORT;
		const uint32_t* srcIndices = static_cast<const uint32_t*>(data);
		std::vector<uint16_t> indices16(count);
		for (uint32_t i = 0; i < count; i++)
		{
			PX_CORE_ASSERT(srcIndices[i] <= 0xFFFF, "Index exceeds uint16_t range on PS Vita.");
			indices16[i] = static_cast<uint16_t>(srcIndices[i]);
		}
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint16_t), indices16.data(), usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
#else
		m_IndexType = GL_UNSIGNED_INT;
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, usage == IndexBufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
#endif
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t size)
	{
		PX_CORE_ASSERT(m_Usage == IndexBufferUsage::Dynamic, "Only Dynamic Index Buffers can be updated.");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);

#ifdef PX_PLATFORM_PSVITA
		const uint32_t* srcIndices = static_cast<const uint32_t*>(data);
		std::vector<uint16_t> indices16(size);
		for (uint32_t i = 0; i < size; i++)
		{
			PX_CORE_ASSERT(srcIndices[i] <= 0xFFFF, "Index exceeds uint16_t range on Vita!");
			indices16[i] = static_cast<uint16_t>(srcIndices[i]);
		}
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size * sizeof(uint16_t), indices16.data());
#else
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size * sizeof(uint32_t), data);
#endif
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