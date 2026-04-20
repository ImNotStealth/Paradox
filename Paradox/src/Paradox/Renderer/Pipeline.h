#pragma once

#include "Paradox/Core/Base.h"

#include "Paradox/Renderer/Shader.h"
#include "Paradox/Renderer/RenderPass.h"

namespace Paradox
{
	struct PipelineProperties
	{
		Shared<Shader> Shader;
		Shared<RenderPass> RenderPass;

		std::string DebugName;
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