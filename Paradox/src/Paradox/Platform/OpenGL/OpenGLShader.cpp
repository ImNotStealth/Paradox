#include "pxpch.h"
#include "OpenGLShader.h"

#include "Paradox/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Paradox/Platform/OpenGL/OpenGLUniformBufferSet.h"
#include "Paradox/Platform/OpenGL/OpenGLTexture2D.h"
#include "Paradox/Platform/OpenGL/OpenGL.h"
#include <glm/gtc/type_ptr.hpp>

namespace Paradox
{
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
		: m_Name(name)
	{
		PX_PROFILE_FUNCTION();
		bool isSpirV = IsFileSpirV(GetBasePath() + vertFilePath);
		PX_CORE_ASSERT(IsFileSpirV(GetBasePath() + fragFilePath) == isSpirV, "Mismatch between vertex and fragment shader, they must both be either SPIR-V or GLSL.");

		std::string vertSource = ReadFile(GetBasePath() + vertFilePath, isSpirV);
		std::string fragSource = ReadFile(GetBasePath() + fragFilePath, isSpirV);

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
		PX_CORE_TRACE("Created Shader: {0} (Program: {1})", m_Name, m_ProgramID);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ProgramID);
		PX_CORE_TRACE("Destroyed Shader: {0}", m_Name);
	}

	void OpenGLShader::Bind()
	{
		PX_PROFILE_FUNCTION();
		PX_CORE_ASSERT(m_ProgramID, "Shader program not created.");
		glUseProgram(m_ProgramID);

		PX_CORE_ASSERT(m_Inputs.empty() || m_Baked, "Shader Inputs must be baked before rendering.");

		for (const auto& [binding, entry] : m_Inputs)
		{
			if (entry.type == ShaderInputType::UniformBuffer)
			{
				Shared<OpenGLUniformBufferSet> uniformBufferSet = std::static_pointer_cast<OpenGLUniformBufferSet>(entry.data[0]);
				Shared<OpenGLUniformBuffer> uniformBuffer = std::static_pointer_cast<OpenGLUniformBuffer>(uniformBufferSet->GetCurrent());

#ifdef PX_PLATFORM_PSVITA
				uint32_t location = glGetUniformBlockIndex(m_ProgramID, entry.name.c_str());
				#ifdef PX_ENABLE_ASSERTS
				if (location == GL_INVALID_INDEX)
				{
					PX_CORE_ERROR("Shader: {0} Program: {1} Location Name: {2}", m_Name, m_ProgramID, entry.name);
					PX_CORE_ASSERT(false, "Failed to find UniformBlockIndex.");
				}
				#endif
				glUniformBlockBinding(m_ProgramID, location, binding);
#endif

				glBindBufferBase(GL_UNIFORM_BUFFER, binding, uniformBuffer->GetBufferID());
			}
			else if (entry.type == ShaderInputType::Texture)
			{
#ifdef PX_PLATFORM_PSVITA
				Shared<OpenGLTexture2D> texture = std::static_pointer_cast<OpenGLTexture2D>(entry.data[0]);
				uint32_t location = glGetUniformLocation(m_ProgramID, entry.name.c_str());
				#ifdef PX_ENABLE_ASSERTS
				if (location == GL_INVALID_INDEX)
				{
					PX_CORE_ERROR("Shader: {0} Program: {1} Location Name: {2}", m_Name, m_ProgramID, entry.name);
					PX_CORE_ASSERT(false, "Failed to find UniformLocation.");
				}
				#endif
				glUniform1i(location, binding);

				glBindSampler(binding, texture->GetSamplerID());
				glActiveTexture(GL_TEXTURE0 + binding);
				glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
#else
				for (size_t textureIndex = 0; textureIndex < entry.data.size(); textureIndex++)
				{
					Shared<OpenGLTexture2D> texture = std::static_pointer_cast<OpenGLTexture2D>(entry.data[textureIndex]);
					glBindSampler(binding + textureIndex, texture->GetSamplerID());
					glBindTextureUnit(binding + textureIndex, texture->GetTextureID());
				}
#endif
			}
		}
	}

	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformBufferInput(uint32_t binding, Shared<UniformBufferSet> ubo, const std::string& name)
	{
		PX_CORE_ASSERT(m_Baked || m_Inputs.count(binding) == 0, "Duplicate binding.");

		ShaderInput input = {};
		input.type = ShaderInputType::UniformBuffer;
		input.binding = binding;
		input.data[0] = ubo;
		input.name = name;
		m_Inputs[binding] = input;
	}

	void OpenGLShader::SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name, uint32_t index)
	{
		if (m_Inputs.count(binding) != 0)
		{
			ShaderInput& input = m_Inputs[binding];

			PX_CORE_ASSERT(name == input.name, "Input names must match for readibility.");

			if (index >= input.data.size())
				input.data.resize(index + 1);

			PX_CORE_ASSERT(m_Baked || !input.data[index], "Texture Index already in use.");
			input.data[index] = texture;
			return;
		}

		ShaderInput input = {};
		input.type = ShaderInputType::Texture;
		input.binding = binding;
		input.data[index] = texture;
		input.name = name;
		m_Inputs[binding] = input;
	}

	std::string OpenGLShader::ReadFile(const std::string& filePath, bool isSpirV)
	{
		std::ifstream file(filePath, std::ios::ate | (isSpirV ? std::ios::binary : std::ios::openmode(0)));
		PX_CORE_ASSERT(file.is_open(), "Failed to open file.");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return std::string(buffer.data(), buffer.size());
	}

	uint32_t OpenGLShader::Compile(const std::string& source, uint32_t type, bool isSpirV)
	{
		PX_PROFILE_FUNCTION();
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

		//On CG, every attrib must be used or their location will be shifted. (This doesn't happen on Vulkan or OpenGL)
		//Ex: Attrib A, B, C, if B is unused, C will be written into B but also converted to that type so data would be messed up.
		//TODO: Maybe move away from manually setting bindings by index all together and instead get some reflection data somehow to verify everything is being used.
		//PX_CORE_INFO("inPosition location: {0}", glGetAttribLocation(m_ProgramID, "inPosition"));
		//PX_CORE_INFO("inColor location: {0}", glGetAttribLocation(m_ProgramID, "inColor"));
		//PX_CORE_INFO("inTexCoord location: {0}", glGetAttribLocation(m_ProgramID, "inTexCoord"));
	}

	bool OpenGLShader::IsFileSpirV(const std::string& filePath)
	{
		return filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".spv";
	}
}