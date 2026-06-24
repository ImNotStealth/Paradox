#pragma once

#include "Paradox/Renderer/Shader.h"

#include "Paradox/Platform/Vulkan/Vulkan.h"

namespace Paradox
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath);
		~VulkanShader();

		void SetUniformBufferInput(uint32_t binding, Shared<UniformBufferSet> ubo, const std::string& name) override;
		void SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name) override;
		void BakeInput() override;

		const ShaderInput& GetInput(uint32_t binding) override { PX_CORE_ASSERT(binding < m_Inputs.size()); return m_Inputs[binding]; }
		const std::unordered_map<uint32_t, ShaderInput>& GetInputs() override { return m_Inputs; }
		bool HasInputs() override { return !m_Inputs.empty(); }
		bool IsBaked() override { return m_Baked; }

		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return m_DescriptorSets; }

		const std::string& GetName() override { return m_Name; }
		const char* GetBasePath() override { return "shaders/desktop/compiled/"; }

		const uint32_t GetStageCount() const { return 2; } // Vertex and Fragment
		const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStages() { return m_ShaderStages; }

		static VkDescriptorType GetVulkanDescriptorType(ShaderInputType type);

	private:
		std::vector<char> ReadFile(const std::string& fileName);
		VkShaderModule CreateShaderModule(const std::string& filePath);

	private:
		std::string m_Name;
		bool m_Baked = false;
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		std::unordered_map<uint32_t, ShaderInput> m_Inputs;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_DescriptorSets;
	};
}