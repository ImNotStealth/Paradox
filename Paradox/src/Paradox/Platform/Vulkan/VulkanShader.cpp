#include "pxpch.h"
#include "VulkanShader.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Renderer/Renderer.h"

namespace Paradox
{
    VulkanShader::VulkanShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
    {
        PX_CORE_TRACE("Vulkan Shader Created: {0}", name);

        VkShaderModule vertModule = CreateShaderModule(vertFilePath);
        VkShaderModule fragModule = CreateShaderModule(fragFilePath);

        m_Name = name;
        m_ShaderStages = new VkPipelineShaderStageCreateInfo[m_StageCount];

        // Init empty create infos (clear memory basically)
        for (uint32_t i = 0; i < m_StageCount; i++)
            m_ShaderStages[i] = VkPipelineShaderStageCreateInfo();

        m_ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        m_ShaderStages[0].module = vertModule;
        m_ShaderStages[0].pName = "main";

        m_ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        m_ShaderStages[1].module = fragModule;
        m_ShaderStages[1].pName = "main";
    }

    VulkanShader::~VulkanShader()
    {
		VkDevice device = VulkanDevice::Get().GetDevice();
        for (uint32_t i = 0; i < m_StageCount; i++)
            vkDestroyShaderModule(device, m_ShaderStages[i].module, nullptr);

        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

        delete[] m_ShaderStages;
        PX_CORE_TRACE("Shader Destroyed: {0}", m_Name);
    }

    void VulkanShader::SetUniform(Shared<UniformBufferSet> uniform)
    {
        m_Uniform = uniform;
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
        VkResult result = vkCreateShaderModule(VulkanDevice::Get().GetDevice(), &createInfo, nullptr, &shaderModule);
        PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Shader module.");

        return shaderModule;
    }

    void VulkanShader::CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		layoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pBindings = &layoutBinding;
        layoutCreateInfo.bindingCount = 1;

        VkResult result = vkCreateDescriptorSetLayout(VulkanDevice::Get().GetDevice(), &layoutCreateInfo, nullptr, &m_DescriptorSetLayout);
		PX_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Descriptor Set Layout.");
    }
}