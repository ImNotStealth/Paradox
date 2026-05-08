#pragma once

#include "Paradox/Renderer/Shader.h"

#include <vulkan/vulkan.h>

namespace Paradox
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
		virtual ~VulkanShader();

		const std::string& GetName() override { return m_Name; }

		const uint32_t GetStageCount() const { return m_StageCount; }
		VkPipelineShaderStageCreateInfo* GetShaderStages() { return m_ShaderStages; }

	private:
		std::vector<char> ReadFile(const std::string& fileName);
		VkShaderModule CreateShaderModule(const std::string& filePath);

	private:
		std::string m_Name;
		const uint32_t m_StageCount = 2;
		VkPipelineShaderStageCreateInfo* m_ShaderStages;
	};
}