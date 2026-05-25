#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/UniformBufferSet.h"

namespace Paradox
{
	class PARADOX_API Shader
	{
	public:
		virtual ~Shader() = default;
	
		virtual void SetUniform(Shared<UniformBufferSet> uniform) = 0;

		virtual const std::string& GetName() = 0;

		static Shared<Shader> Create(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
	};
}