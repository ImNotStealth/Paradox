#pragma once

#include "Paradox/Core/Base.h"

#include "Paradox/Renderer/Shader.h"
#include "Paradox/Renderer/RenderPass.h"

namespace Paradox
{
	struct PipelineProperties
	{
		Shared<Shader> shader;
		Shared<RenderPass> renderPass;

		std::string debugName;
	};

	// PARADOX_API is temporary
	class PARADOX_API Pipeline
	{
	public:
		~Pipeline() = default;

		virtual PipelineProperties& GetProperties() = 0;

		static Shared<Pipeline> Create(const PipelineProperties& props);
	};
}