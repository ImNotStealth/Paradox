#include "pxpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Paradox
{
	OpenGLPipeline::OpenGLPipeline(const PipelineProperties& props)
		: m_Props(props)
	{
		glGenVertexArrays(1, &m_VertexArrayID);
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void OpenGLPipeline::Bind()
	{
		glBindVertexArray(m_VertexArrayID);

		uint32_t i = 0;
		for (const VertexBufferElement& element : m_Props.layout.GetElements())
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.componentCount, GL_FLOAT, GL_FALSE, m_Props.layout.GetStride(), (const void*)(intptr_t)element.offset);
			i++;
		}
	}

	void OpenGLPipeline::Unbind()
	{
		glBindVertexArray(0);
	}

	unsigned int OpenGLPipeline::GetOpenGLFormat(VertexBufferDataType type)
	{
		switch (type)
		{
		case VertexBufferDataType::Float: return GL_FLOAT;
		case VertexBufferDataType::Float2: return GL_FLOAT;
		case VertexBufferDataType::Float3: return GL_FLOAT;
		case VertexBufferDataType::Float4: return GL_FLOAT;
		}
		PX_CORE_ASSERT(false, "Invalid VertexBufferDataType.");
		return GL_NONE;
	}
}