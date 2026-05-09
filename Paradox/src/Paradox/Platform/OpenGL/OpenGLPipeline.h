#pragma once

#include "Paradox/Renderer/Pipeline.h"

namespace Paradox
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineProperties& props)
			: m_Props(props) { }

		PipelineProperties& GetProperties() override { return m_Props; }

	private:
		PipelineProperties m_Props;
	};
}