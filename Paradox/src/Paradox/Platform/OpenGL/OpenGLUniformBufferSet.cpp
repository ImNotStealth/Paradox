#include "pxpch.h"
#include "OpenGLUniformBufferSet.h"

namespace Paradox
{
	OpenGLUniformBufferSet::OpenGLUniformBufferSet(uint32_t size)
	{
		m_UniformBuffer = UniformBuffer::Create(size);
	}
}