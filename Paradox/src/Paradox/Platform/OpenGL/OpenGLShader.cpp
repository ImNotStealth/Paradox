#include "pxpch.h"
#include "OpenGLShader.h"

#include "Paradox/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLUniformBufferSet.h"
#include "Paradox/Platform/OpenGL/OpenGLTexture.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"
#include <glm/gtc/type_ptr.hpp>

namespace Paradox
{
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
		: m_Name(name)
	{
		bool isSpirV = IsFileSpirV(GetBasePath() + vertFilePath);
		PX_CORE_ASSERT(IsFileSpirV(GetBasePath() + fragFilePath) == isSpirV, "Mismatch between vertex and fragment shader, they must both be either SPIR-V or GLSL.");

		std::vector<char> vertSource = ReadFile(GetBasePath() + vertFilePath, isSpirV);
		std::vector<char> fragSource = ReadFile(GetBasePath() + fragFilePath, isSpirV);

#ifndef PX_PLATFORM_PSVITA
		uint32_t vertID = Compile(vertSource, GL_VERTEX_SHADER, isSpirV);
		uint32_t fragID = Compile(fragSource, GL_FRAGMENT_SHADER, isSpirV);
#else
		uint32_t vertID = Compile(vertSource, GL_CG_VERTEX_SHADER_EXT, isSpirV);
		uint32_t fragID = Compile(fragSource, GL_CG_FRAGMENT_SHADER_EXT, isSpirV);
#endif
		if (!vertID || !fragID)
		{
			PX_CORE_ERROR("Failed to compile fragment or vertex shader of: {0}", name);
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

		for (const auto& [binding, entry] : m_Inputs)
		{
			if (entry.type == ShaderInputType::UniformBuffer)
			{
				Shared<OpenGLUniformBufferSet> uniformBufferSet = std::static_pointer_cast<OpenGLUniformBufferSet>(entry.data);
				Shared<OpenGLUniformBuffer> uniformBuffer = std::static_pointer_cast<OpenGLUniformBuffer>(uniformBufferSet->GetCurrent());

#ifdef PX_PLATFORM_PSVITA
				uint32_t location = glGetUniformBlockIndex(m_ProgramID, entry.name.c_str());
				glUniformBlockBinding(m_ProgramID, location, binding);
#endif

				glBindBufferBase(GL_UNIFORM_BUFFER, binding, uniformBuffer->GetBufferID());
			}
			else if (entry.type == ShaderInputType::Texture)
			{
				Shared<OpenGLTexture> texture = std::static_pointer_cast<OpenGLTexture>(entry.data);
				glBindTextureUnit(binding, texture->GetTextureID());
			}
		}
	}

	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformBufferInput(uint32_t binding, Shared<UniformBufferSet> ubo, const std::string& name)
	{
		PX_CORE_ASSERT(m_Inputs.count(binding) == 0, "Duplicate binding.");

		ShaderInput input = {};
		input.type = ShaderInputType::UniformBuffer;
		input.binding = binding;
		input.data = ubo;
		input.name = name;
		m_Inputs.emplace(binding, input);
	}

	void OpenGLShader::SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name)
	{
		PX_CORE_ASSERT(m_Inputs.count(binding) == 0, "Duplicate binding.");

		ShaderInput input = {};
		input.type = ShaderInputType::Texture;
		input.binding = binding;
		input.data = texture;
		input.name = name;
		m_Inputs.emplace(binding, input);
	}

	std::vector<char> OpenGLShader::ReadFile(const std::string& filePath, bool isSpirV)
	{
		std::ifstream file(filePath, std::ios::ate | (isSpirV ? std::ios::binary : std::ios::openmode(0)));
		PX_CORE_ASSERT(file.is_open(), "Failed to open file.");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	uint32_t OpenGLShader::Compile(const std::vector<char>& source, uint32_t type, bool isSpirV)
	{
		uint32_t shaderID = glCreateShader(type);

#ifndef PX_PLATFORM_PSVITA
		if (isSpirV)
		{
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), (GLsizei)source.size());
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
		}
		else
		{
			const char* sourceCStr = source.data();
			glShaderSource(shaderID, 1, &sourceCStr, nullptr);
			glCompileShader(shaderID);
		}
#else
		const char* sourceCStr = source.data();
		glShaderSource(shaderID, 1, &sourceCStr, nullptr);
		glCompileShader(shaderID);
#endif

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

	bool OpenGLShader::IsFileSpirV(const std::string& filePath)
	{
		return filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".spv";
	}
}