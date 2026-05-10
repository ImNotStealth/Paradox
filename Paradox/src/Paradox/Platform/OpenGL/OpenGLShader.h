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

		const std::string& GetName() override { return m_Name; }
		
	private:
		std::string ReadFile(const std::string& filePath);
		[[nodiscard]] uint32_t Compile(const std::string& source, uint32_t type);
		void CreateProgram(uint32_t vertID, uint32_t fragID);
	
	private:
		std::string m_Name;
		uint32_t m_ProgramID = 0;
	};
}