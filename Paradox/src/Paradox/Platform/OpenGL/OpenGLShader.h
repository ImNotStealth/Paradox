#pragma once

#include "Paradox/Renderer/Shader.h"

namespace Paradox
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
			: m_Name(name) { }
		
		const std::string& GetName() override { return m_Name; }
	
	private:
		std::string m_Name;
	};
}