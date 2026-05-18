#pragma once

#include "Paradox/Renderer/Pipeline.h"

namespace Paradox
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineProperties& props);
		~OpenGLPipeline() override;

		PipelineProperties& GetProperties() override { return m_Props; }

		void Bind();
		void Unbind();

	private:
		unsigned int GetOpenGLFormat(VertexBufferDataType type);

	private:
		PipelineProperties m_Props;
		uint32_t m_VertexArrayID = 0;
	};
}