#pragma once

#include "Paradox/Renderer/Shader.h"

#include "Paradox/Renderer/UniformBufferSet.h"

namespace Paradox
{
	class OpenGLShader : public Shader
	{
	public:
		struct UniformEntry
		{
			Shared<UniformBufferSet> uniform = nullptr;
			uint32_t binding;
			std::string name;
		};

		OpenGLShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
		~OpenGLShader() override;

		void Bind();
		void Unbind();

		void SetUniformBufferInput(uint32_t binding, Shared<UniformBufferSet> ubo, const std::string& name) override;
		void SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name) override;
		void BakeInput() override { PX_CORE_ASSERT(!m_Baked, "Shader Inputs already baked."); m_Baked = true; }

		const ShaderInput& GetInput(uint32_t binding) override { PX_CORE_ASSERT(binding < m_Inputs.size()); return m_Inputs[binding]; }
		const std::unordered_map<uint32_t, ShaderInput>& GetInputs() override { return m_Inputs; }
		bool HasInputs() override { return !m_Inputs.empty(); }
		bool IsBaked() override { return m_Baked; }

		const std::string& GetName() override { return m_Name; }
		const char* GetBasePath() override
		{
#ifndef PX_PLATFORM_PSVITA
			return "shaders/desktop/";
#else
			return "shaders/psvita/";
#endif
		}
		
	private:
		std::vector<char> ReadFile(const std::string& filePath, bool isSpirV);
		[[nodiscard]] uint32_t Compile(const std::vector<char>& source, uint32_t type, bool isSpirV);
		void CreateProgram(uint32_t vertID, uint32_t fragID);
		bool IsFileSpirV(const std::string& filePath);
	
	private:
		std::string m_Name;
		bool m_Baked = false;
		uint32_t m_ProgramID = 0;
		std::unordered_map<uint32_t, ShaderInput> m_Inputs;
	};
}