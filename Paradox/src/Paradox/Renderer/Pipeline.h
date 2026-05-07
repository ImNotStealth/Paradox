#pragma once

#include "Paradox/Core/Base.h"

#include "Paradox/Renderer/Shader.h"
#include "Paradox/Renderer/RenderPass.h"
#include "Paradox/Renderer/VertexBufferLayout.h"

namespace Paradox
{
	enum class CullMode
	{
		None = 0, Front, Back, FrontAndBack
	};

	struct PipelineProperties
	{
		Shared<Shader> shader;
		Shared<RenderPass> renderPass;
		VertexBufferLayout layout;
		bool wireframe = false;
		float wireframeWidth = 1.f;
		CullMode cullMode = CullMode::Back;

		std::string debugName;
	};

	// PARADOX_API is temporary
	class PARADOX_API Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineProperties& GetProperties() = 0;

		static Shared<Pipeline> Create(const PipelineProperties& props);
	};
}