#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/UniformBufferSet.h"

namespace Paradox
{
	class PARADOX_API Shader
	{
	public:
		virtual ~Shader() = default;
	
		virtual void SetUniforms(const std::vector<std::pair<uint32_t, Shared<UniformBufferSet>>>& uniforms) = 0;
		virtual Shared<UniformBufferSet> GetUniform(uint32_t binding) = 0;
		virtual bool HasUniforms() = 0;

		virtual const std::string& GetName() = 0;

		static Shared<Shader> Create(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
	};
}