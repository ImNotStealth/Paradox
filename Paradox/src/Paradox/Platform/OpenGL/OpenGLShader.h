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

		void SetUniforms(const std::vector<std::tuple<uint32_t, Shared<UniformBufferSet>, std::string>>& uniforms) override;
		Shared<UniformBufferSet> GetUniform(uint32_t binding) override { return m_Uniforms[binding].uniform; }
		bool HasUniforms() override { return !m_Uniforms.empty(); }

		void SetTexture(Shared<Texture> texture) override {};
		Shared<Texture> GetTexture() override { return nullptr; }

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
		uint32_t m_ProgramID = 0;
		std::unordered_map<uint32_t, UniformEntry> m_Uniforms;
	};
}