#include "pxpch.h"
#include "VulkanShader.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Renderer/Renderer.h"

namespace Paradox
{
    VulkanShader::VulkanShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
    {
        PX_CORE_TRACE("Vulkan Shader Created: {0}", name);

        VkShaderModule vertModule = CreateShaderModule(GetBasePath() + vertFilePath + ".spv");
        VkShaderModule fragModule = CreateShaderModule(GetBasePath() + fragFilePath + ".spv");

        m_Name = name;

        m_ShaderStages.reserve(GetStageCount());
        VkPipelineShaderStageCreateInfo vertStageInfo = {};
        vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module = vertModule;
        vertStageInfo.pName = "main";
        m_ShaderStages.push_back(vertStageInfo);

        VkPipelineShaderStageCreateInfo fragStageInfo = {};
        fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageInfo.module = fragModule;
        fragStageInfo.pName = "main";
        m_ShaderStages.push_back(fragStageInfo);
    }

    VulkanShader::~VulkanShader()
    {
		VkDevice device = VulkanDevice::Get().GetDevice();
        for (uint32_t i = 0; i < GetStageCount(); i++)
            vkDestroyShaderModule(device, m_ShaderStages[i].module, nullptr);

        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

        PX_CORE_TRACE("Shader Destroyed: {0}", m_Name);
    }

    void VulkanShader::SetUniforms(const std::vector<std::tuple<uint32_t, Shared<UniformBufferSet>, std::string>>& uniforms)
    {
        PX_CORE_ASSERT(m_Uniforms.empty(), "Didn't test if resetting uniforms works yet.");

        m_Uniforms.clear();
		m_Uniforms.reserve(uniforms.size());
        for (const auto& [binding, uniform, name] : uniforms)
        {
			PX_CORE_ASSERT(m_Uniforms.count(binding) == 0, "Duplicate uniform binding.");

            UniformEntry& entry = m_Uniforms[binding];
            entry.binding = binding;
			entry.uniform = uniform;
        }

        CreateDescriptorSetLayout();

        VulkanDevice::Get().AllocateDescriptorSets(m_DescriptorSetLayout, Renderer::GetMaxFramesInFlight(), m_DescriptorSets);
    }

    std::vector<char> VulkanShader::ReadFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        PX_CORE_ASSERT(file.is_open(), "Failed to open file.");

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    VkShaderModule VulkanShader::CreateShaderModule(const std::string& filePath)
    {
        std::vector<char> shaderCode = ReadFile(filePath);

        // This magic number is part of the SPIR-V spec, https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html
        uint32_t magic = 0;
        memcpy(&magic, shaderCode.data(), sizeof(uint32_t));
		PX_CORE_ASSERT(magic == 0x07230203, "Shader file is not a SPIR-V binary.");

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule = {};
        VK_CHECK_RESULT(vkCreateShaderModule(VulkanDevice::Get().GetDevice(), &createInfo, nullptr, &shaderModule));
        VulkanUtils::SetDebugName(VK_OBJECT_TYPE_SHADER_MODULE, shaderModule, m_Name + " (Shader Module)");

        return shaderModule;
    }

    void VulkanShader::CreateDescriptorSetLayout()
    {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(m_Uniforms.size());
        for (const auto& [binding, entry] : m_Uniforms)
        {
            VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
            layoutBinding.binding = binding;
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            layoutBinding.descriptorCount = 1;

            //TODO: Set for both vertex and fragment, this is to have parity with OpenGL. Check later on if this is ok or not
            layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            layoutBinding.pImmutableSamplers = nullptr;
        }
        
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pBindings = layoutBindings.data();
        layoutCreateInfo.bindingCount = layoutBindings.size();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VulkanDevice::Get().GetDevice(), &layoutCreateInfo, nullptr, &m_DescriptorSetLayout));
        VulkanUtils::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, m_DescriptorSetLayout, m_Name + " (Descriptor Set Layout)");
    }
}