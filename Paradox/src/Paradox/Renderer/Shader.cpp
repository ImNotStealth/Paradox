#include "pxpch.h"
#include "Shader.h"

namespace Paradox
{
    Shader::Shader(const std::string& name, VkDevice device, const std::string& vertFilePath, const std::string& fragFilePath)
        : m_Name(name), m_Device(device)
    {
        PX_CORE_TRACE("Shader Created: {0}", name);

        VkShaderModule vertModule = CreateShaderModule(vertFilePath);
        VkShaderModule fragModule = CreateShaderModule(fragFilePath);

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

    Shader::~Shader()
    {
        for (uint32_t i = 0; i < m_StageCount; i++)
            vkDestroyShaderModule(m_Device, m_ShaderStages[i].module, nullptr);

        delete[] m_ShaderStages;
        PX_CORE_TRACE("Shader Destroyed: {0}", m_Name);
    }

    std::vector<char> Shader::ReadFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        PX_ASSERT(file.is_open(), "Failed to open file.");

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    VkShaderModule Shader::CreateShaderModule(const std::string& filePath)
    {
        std::vector<char> shaderCode = ReadFile(filePath);
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule = {};
        VkResult result = vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule);
        PX_ASSERT(result == VK_SUCCESS, "Failed to create Shader module.");

        return shaderModule;
    }
}