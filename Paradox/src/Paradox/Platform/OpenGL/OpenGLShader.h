#pragma once

#include "Paradox/Renderer/Shader.h"

namespace Paradox
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
		~OpenGLShader() override;

		void Bind();
		void Unbind();

		void SetUniform(Shared<UniformBufferSet> uniform) override { PX_CORE_ASSERT(false, "Not implemented."); }

		const std::string& GetName() override { return m_Name; }
		
	private:
		std::vector<char> ReadFile(const std::string& filePath, bool isSpirV);
		[[nodiscard]] uint32_t Compile(const std::vector<char>& source, uint32_t type, bool isSpirV);
		void CreateProgram(uint32_t vertID, uint32_t fragID);
		bool IsFileSpirV(const std::string& filePath);
	
	private:
		std::string m_Name;
		uint32_t m_ProgramID = 0;
	};
}