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

		void SetUniform(Shared<UniformBufferSet> uniform) override;

		//CHECK IF TEMPORARY
		Shared<UniformBufferSet> GetUniform() { return m_Uniform; }
		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		VkDescriptorSet& GetDescriptorSet(uint32_t frameIndex) { return m_DescriptorSets[frameIndex]; }

		const std::string& GetName() override { return m_Name; }

		const uint32_t GetStageCount() const { return m_StageCount; }
		VkPipelineShaderStageCreateInfo* GetShaderStages() { return m_ShaderStages; }

	private:
		std::vector<char> ReadFile(const std::string& fileName);
		VkShaderModule CreateShaderModule(const std::string& filePath);
		void CreateDescriptorSetLayout();

	private:
		std::string m_Name;
		const uint32_t m_StageCount = 2;
		VkPipelineShaderStageCreateInfo* m_ShaderStages;
		Shared<UniformBufferSet> m_Uniform = nullptr;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_DescriptorSets;
	};
}