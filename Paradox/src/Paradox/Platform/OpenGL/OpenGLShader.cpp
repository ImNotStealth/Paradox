#include "pxpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace Paradox
{
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
		: m_Name(name)
	{
		std::string vertSource = ReadFile(vertFilePath);
		std::string fragSource = ReadFile(fragFilePath);
		//TODO: For VitaGL, the sample uses GL_CG_VERTEX_SHADER_EXT and GL_CG_FRAGMENT_SHADER_EXT instead.
		uint32_t vertID = Compile(vertSource, GL_VERTEX_SHADER);
		uint32_t fragID = Compile(fragSource, GL_FRAGMENT_SHADER);
		if (!vertID || !fragID)
		{
			PX_CORE_ERROR("Failed to compile fragment of vertex shader of: {0}", name);
			return;
		}
		CreateProgram(vertID, fragID);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ProgramID);
	}

	void OpenGLShader::Bind()
	{
		PX_CORE_ASSERT(m_ProgramID, "Shader program not created.");
		glUseProgram(m_ProgramID);
	}

	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
	}

	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		std::string source;

		std::ifstream file(filePath, std::ios::in);
		if (file)
		{
			file.seekg(0, std::ios::end);
			size_t fileSize = file.tellg();
			if (fileSize != -1)
			{
				source.resize(fileSize);
				file.seekg(0, std::ios::beg);
				file.read(&source[0], fileSize);
			}
			else
				PX_CORE_ERROR("Failed to read file: {0}", filePath);
		}
		else
			PX_CORE_ERROR("Failed to open file: {0}", filePath);
		return source;
	}

	uint32_t OpenGLShader::Compile(const std::string& source, uint32_t type)
	{
		uint32_t shaderID = glCreateShader(type);
		const char* sourceCStr = source.c_str();
		glShaderSource(shaderID, 1, &sourceCStr, nullptr);
		glCompileShader(shaderID);

		int success;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
			PX_CORE_ERROR("Failed to compile shader: {0}", infoLog);
			return 0;
		}

		return shaderID;
	}

	void OpenGLShader::CreateProgram(uint32_t vertID, uint32_t fragID)
	{
		m_ProgramID = glCreateProgram();
		glAttachShader(m_ProgramID, vertID);
		glAttachShader(m_ProgramID, fragID);
		glLinkProgram(m_ProgramID);

		int success;
		glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
			PX_CORE_ERROR("Failed to link shader: {0}", infoLog);
			return;
		}

		glDeleteShader(vertID);
		glDeleteShader(fragID);
	}
}