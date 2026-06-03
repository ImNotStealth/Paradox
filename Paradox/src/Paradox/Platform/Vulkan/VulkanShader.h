#pragma once

#include "Paradox/Renderer/Shader.h"

#include <vulkan/vulkan.h>

namespace Paradox
{
	class VulkanShader : public Shader
	{
	public:
		struct UniformEntry
		{
			Shared<UniformBufferSet> uniform = nullptr;
			uint32_t binding;
		};

		VulkanShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
		~VulkanShader();

		void SetUniforms(const std::vector<std::tuple<uint32_t, Shared<UniformBufferSet>, std::string>>& uniforms) override;
		Shared<UniformBufferSet> GetUniform(uint32_t binding) override { return m_Uniforms[binding].uniform; }
		bool HasUniforms() override { return !m_Uniforms.empty(); }

		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }
		const std::unordered_map<uint32_t, UniformEntry>& GetUniforms() const { return m_Uniforms; }

		const std::string& GetName() override { return m_Name; }
		const char* GetBasePath() override { return "shaders/desktop/compiled/"; }

		const uint32_t GetStageCount() const { return 2; } // Vertex and Fragment
		const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() { return m_ShaderStages; }

	private:
		std::vector<char> ReadFile(const std::string& fileName);
		VkShaderModule CreateShaderModule(const std::string& filePath);
		void CreateDescriptorSetLayout();

	private:
		std::string m_Name;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		std::unordered_map<uint32_t, UniformEntry> m_Uniforms;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_DescriptorSets;
	};
}