#pragma once

#include "Paradox/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Paradox
{
	class PARADOX_API Shader
	{
	public:
		Shader(const std::string& name, VkDevice device, const std::string& vertFilePath, const std::string& fragFilePath);
		~Shader();

		const uint32_t GetStageCount() const { return m_StageCount; }
		VkPipelineShaderStageCreateInfo* GetShaderStages() { return m_ShaderStages; }

	private:
		std::vector<char> ReadFile(const std::string& fileName);
		VkShaderModule CreateShaderModule(const std::string& filePath);

	private:
		std::string m_Name;
		const uint32_t m_StageCount = 2;
		VkPipelineShaderStageCreateInfo* m_ShaderStages;

		VkDevice m_Device;
	};
}