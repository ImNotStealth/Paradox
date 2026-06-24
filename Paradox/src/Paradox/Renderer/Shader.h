#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/UniformBufferSet.h"
#include "Paradox/Renderer/Texture.h"

namespace Paradox
{
	enum class ShaderInputType
	{
		UniformBuffer,
		Texture
	};

	struct ShaderInput
	{
		ShaderInputType type;
		uint32_t binding;
		Shared<void> data;
		std::string name;
	};

	class PARADOX_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void SetUniformBufferInput(uint32_t binding, Shared<UniformBufferSet> ubo, const std::string& name) = 0;
		virtual void SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name) = 0;
		virtual void BakeInput() = 0;

		virtual const ShaderInput& GetInput(uint32_t binding) = 0;
		virtual const std::unordered_map<uint32_t, ShaderInput>& GetInputs() = 0;
		virtual bool HasInputs() = 0;

		virtual const std::string& GetName() = 0;
		virtual const char* GetBasePath() = 0;

		static Shared<Shader> Create(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
	};
}